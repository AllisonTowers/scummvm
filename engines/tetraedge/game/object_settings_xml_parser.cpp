/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "tetraedge/game/object_settings_xml_parser.h"

namespace Tetraedge {

bool ObjectSettingsXmlParser::parserCallback_ObjectsSettings(ParserNode *node) {
	// Nothing to do, data handled in the child keys.
	return true;
}

bool ObjectSettingsXmlParser::parserCallback_Object(ParserNode *node) {
	const Common::String &objname = node->values["name"];
	_curObject._name = objname;
	_objectSettings->setVal(objname, _curObject);
	_curObject.clear();
	return true;
}

bool ObjectSettingsXmlParser::parserCallback_modelFileName(ParserNode *node) {
	_textTagType = TagModelFileName;
	return true;
}

bool ObjectSettingsXmlParser::parserCallback_defaultScale(ParserNode *node) {
	_textTagType = TagDefaultScale;
	return true;
}

bool ObjectSettingsXmlParser::textCallback(const Common::String &val) {
	switch (_textTagType) {
		case TagModelFileName:
			_curObject._modelFileName = val;
			break;
		case TagDefaultScale:
		{
			bool result = _curObject._defaultScale.parse(val);
			if (!result)
				warning("Failed to parse Object defaultScale from %s", val.c_str());
			break;
		}
		default:
			error("should only see text for model file name or scale");
	}
	return true;
}

} // end namespace Tetraedge
