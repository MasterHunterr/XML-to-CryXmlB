/*
XML to CryXmlB converter
Copyright (c) 2023 Mohammed Hussin (MasterHunterr)
MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <map>

#include "tinyxml2.h"

// Define the same structs as in main.cpp
struct cry_xml_node_t {
	int32_t name_offset;
	int32_t content_offset;
	int16_t attribute_count;
	int16_t child_count;
	int32_t parent_id;
	int32_t first_attr_idx;
	int32_t first_child_idx;
	int32_t reserved;
};

struct cry_xml_ref_t {
	int32_t name_offset;
	int32_t value_offset;
};

struct read_file_result_t {
	unsigned char* data;
	uint64_t size;
};

// Declare these functions as external since they're defined in main.cpp
extern read_file_result_t read_file(const char* filename);
extern bool write_file(const char* filename, const unsigned char* data, size_t size);

// Helper function to write a 32-bit integer in little-endian format
void write_int32(std::vector<unsigned char>& buffer, int32_t value) {
	buffer.push_back(value & 0xFF);
	buffer.push_back((value >> 8) & 0xFF);
	buffer.push_back((value >> 16) & 0xFF);
	buffer.push_back((value >> 24) & 0xFF);
}

// Helper function to write a 16-bit integer in little-endian format
void write_int16(std::vector<unsigned char>& buffer, int16_t value) {
	buffer.push_back(value & 0xFF);
	buffer.push_back((value >> 8) & 0xFF);
}

// Helper function to add a null-terminated string to the data table
int32_t add_string_to_data_table(std::vector<char>& data_table, const char* str) {
	if (!str) {
		str = ""; // Use empty string for null pointers
	}

	int32_t offset = static_cast<int32_t>(data_table.size());
	// Add the string including the null terminator
	while (*str) {
		data_table.push_back(*str++);
	}
	data_table.push_back('\0'); // Add null terminator

	return offset;
}

// Recursive function to process XML nodes
void process_xml_node(tinyxml2::XMLElement* element, int32_t parent_id,
	std::vector<cry_xml_node_t>& node_table,
	std::vector<cry_xml_ref_t>& attr_table,
	std::vector<uint32_t>& child_table,
	std::vector<char>& data_table,
	std::map<tinyxml2::XMLElement*, int32_t>& node_indices) {

	// Create a new node
	cry_xml_node_t node = {};

	// Store the current node index
	int32_t node_idx = static_cast<int32_t>(node_table.size());
	node_indices[element] = node_idx;

	// Set parent ID
	node.parent_id = parent_id;

	// Add node name to data table
	node.name_offset = add_string_to_data_table(data_table, element->Name());

	// Add node content to data table
	node.content_offset = add_string_to_data_table(data_table, element->GetText());

	// Process attributes
	node.first_attr_idx = static_cast<int32_t>(attr_table.size());
	node.attribute_count = 0;

	const tinyxml2::XMLAttribute* attr = element->FirstAttribute();
	while (attr) {
		cry_xml_ref_t attr_ref = {};
		attr_ref.name_offset = add_string_to_data_table(data_table, attr->Name());
		attr_ref.value_offset = add_string_to_data_table(data_table, attr->Value());
		attr_table.push_back(attr_ref);
		node.attribute_count++;
		attr = attr->Next();
	}

	// Process child elements
	node.first_child_idx = static_cast<int32_t>(child_table.size());
	node.child_count = 0;

	tinyxml2::XMLElement* child = element->FirstChildElement();
	while (child) {
		// Add child index placeholder (will be filled later)
		child_table.push_back(0); // Placeholder
		node.child_count++;
		child = child->NextSiblingElement();
	}

	// Add the node to the node table
	node_table.push_back(node);

	// Process children recursively
	child = element->FirstChildElement();
	int child_index = 0;
	while (child) {
		// Process the child node
		process_xml_node(child, node_idx, node_table, attr_table, child_table, data_table, node_indices);

		// Update the child table with the actual index of the child node
		child_table[node.first_child_idx + child_index] = node_indices[child];
		child_index++;
		child = child->NextSiblingElement();
	}
}

void convert_xml_to_cryxmlb(const char* filename) {
	// Read the XML file
	read_file_result_t xml_file = read_file(filename);
	if (!xml_file.data || xml_file.size == 0) {
		return;
	}

	// Check if the file is already in CryXmlB format
	if (xml_file.size > 0 && xml_file.data[0] == 'C') {
		fprintf(stdout, "File %s is already in CryXmlB format\n", filename);
		free(xml_file.data);
		return;
	}

	// Parse the XML
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error = doc.Parse((const char*)xml_file.data, xml_file.size);
	if (error != tinyxml2::XML_SUCCESS) {
		fprintf(stderr, "Error parsing XML file %s: %s\n", filename, doc.ErrorStr());
		free(xml_file.data);
		return;
	}

	// Free the original file data as we no longer need it
	free(xml_file.data);

	// Get the root element
	tinyxml2::XMLElement* root = doc.RootElement();
	if (!root) {
		fprintf(stderr, "No root element found in XML file %s\n", filename);
		return;
	}

	// Create tables for CryXmlB format
	std::vector<cry_xml_node_t> node_table;
	std::vector<cry_xml_ref_t> attr_table;
	std::vector<uint32_t> child_table;
	std::vector<char> data_table;
	std::map<tinyxml2::XMLElement*, int32_t> node_indices;

	// Process the XML tree
	process_xml_node(root, -1, node_table, attr_table, child_table, data_table, node_indices);

	// Create the output buffer
	std::vector<unsigned char> output_buffer;

	// Write the header
	const char* header = "CryXmlB";
	output_buffer.insert(output_buffer.end(), header, header + strlen(header) + 1); // Include null terminator

	// Calculate offsets
	uint32_t header_size = static_cast<uint32_t>(output_buffer.size() + 8 * sizeof(int32_t)); // Header + 8 int32 values
	uint32_t node_table_offset = header_size;
	uint32_t node_table_size = static_cast<uint32_t>(node_table.size() * sizeof(cry_xml_node_t));
	uint32_t attr_table_offset = node_table_offset + node_table_size;
	uint32_t attr_table_size = static_cast<uint32_t>(attr_table.size() * sizeof(cry_xml_ref_t));
	uint32_t child_table_offset = attr_table_offset + attr_table_size;
	uint32_t child_table_size = static_cast<uint32_t>(child_table.size() * sizeof(uint32_t));
	uint32_t data_table_offset = child_table_offset + child_table_size;
	uint32_t data_table_size = static_cast<uint32_t>(data_table.size());
	uint32_t total_size = data_table_offset + data_table_size;

	// Write file size
	write_int32(output_buffer, total_size);

	// Write table offsets and sizes
	write_int32(output_buffer, node_table_offset);
	write_int32(output_buffer, static_cast<int32_t>(node_table.size()));
	write_int32(output_buffer, attr_table_offset);
	write_int32(output_buffer, static_cast<int32_t>(attr_table.size()));
	write_int32(output_buffer, child_table_offset);
	write_int32(output_buffer, static_cast<int32_t>(child_table.size()));
	write_int32(output_buffer, data_table_offset);
	write_int32(output_buffer, data_table_size);

	// Write node table
	for (const auto& node : node_table) {
		write_int32(output_buffer, node.name_offset);
		write_int32(output_buffer, node.content_offset);
		write_int16(output_buffer, node.attribute_count);
		write_int16(output_buffer, node.child_count);
		write_int32(output_buffer, node.parent_id);
		write_int32(output_buffer, node.first_attr_idx);
		write_int32(output_buffer, node.first_child_idx);
		write_int32(output_buffer, node.reserved);
	}

	// Write attribute table
	for (const auto& attr : attr_table) {
		write_int32(output_buffer, attr.name_offset);
		write_int32(output_buffer, attr.value_offset);
	}

	// Write child table
	for (const auto& child : child_table) {
		write_int32(output_buffer, child);
	}

	// Write data table
	output_buffer.insert(output_buffer.end(), data_table.begin(), data_table.end());

	// Create backup of the original file
	const char* ext_str = "xml.bak";
	char* backup_name = (char*)malloc(strlen(filename) + strlen(ext_str) + 2); // +2 for the dot and null terminator
	if (!backup_name) {
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}
	sprintf(backup_name, "%s.%s", filename, ext_str);

	// Rename the original file to backup
	if (rename(filename, backup_name) != 0) {
		fprintf(stderr, "Error creating backup file %s\n", backup_name);
		free(backup_name);
		return;
	}
	free(backup_name);

	// Write the CryXmlB file
	if (!write_file(filename, output_buffer.data(), output_buffer.size())) {
		fprintf(stderr, "Error writing CryXmlB file %s\n", filename);
	}
	else {
		fprintf(stdout, "Successfully converted %s to CryXmlB format\n", filename);
	}
}