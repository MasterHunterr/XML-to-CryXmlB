# CryXmlB to XML Converter

![Version](https://img.shields.io/badge/Version-2.0-blue) ![License](https://img.shields.io/badge/License-MIT-green)

## Developed by Mohammed Hussin (MasterHunterr) - 2025

## Overview

CryXmlB to XML Converter is a powerful utility designed to convert between CryXmlB binary format and standard XML format. This tool is essential for game developers and modders working with CryEngine-based games, allowing seamless conversion between these formats for easier editing and integration.

## Features

### Bidirectional Conversion

- **XML to CryXmlB**: Convert standard XML files back to the CryXmlB binary format

- If you want me to convert CryXmlB to XML, you can find it here: [https://github.com/MasterHunterr/CryXmlB](https://github.com/MasterHunterr/CryXmlB)

### Enhanced Functionality

- **Automatic Format Detection**: Automatically detects file format and applies the appropriate conversion
- **Batch Processing**: Convert multiple files at once with a single command
- **Backup Creation**: Automatically creates backups of original files before conversion
- **Detailed Logging**: Comprehensive error reporting and conversion status

### Performance Improvements

- **Optimized Memory Usage**: Efficient handling of large XML structures
- **Fast Processing**: Significantly improved conversion speed compared to previous versions
- **Robust Error Handling**: Better recovery from malformed XML files

## Usage

darg and drop files or folders onto the executable to convert them.

### Examples

```
# Convert a single CryXmlB file to XML
CryXmlB.exe game_config.xml

# Convert a single XML file to CryXmlB format
CryXmlB.exe game_config.xml

# Convert all XML files in a directory
CryXmlB.exe -b C:\GameMods\configs\

# Convert all XML files in a directory and its subdirectories
CryXmlB.exe -b -r C:\GameMods\configs\
```

## File Format Support

### CryXmlB Format

The CryXmlB format is a binary representation of XML data used in CryEngine-based games. It consists of:

- Header section with format identifier
- Node table containing element structure
- Attribute table for element properties
- Child index table for hierarchical relationships
- Data table containing all string values

### XML Format

Standard XML format with support for:

- Element hierarchies
- Attributes
- Text content
- Comments (preserved during conversion)

## 2025 Improvements

### Technical Enhancements

- **Unicode Support**: Full UTF-8 encoding support for international character sets
- **Memory Optimization**: Reduced memory footprint for processing large files
- **Streaming Support**: Process files larger than available RAM
- **Multi-threading**: Parallel processing for batch conversions

### User Experience

- **Progress Indicators**: Visual feedback during lengthy conversions
- **Improved Error Messages**: More descriptive error reporting
- **Validation**: XML schema validation before conversion
- **Smart Backup Management**: Configurable backup retention policy

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Uses TinyXML2 for XML parsing
- Special thanks to the CryEngine modding community

---

© 2025 Mohammed Hussin (MasterHunterr). All rights reserved.
