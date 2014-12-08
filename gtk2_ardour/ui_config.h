/*
    Copyright (C) 2000-2007 Paul Davis

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef __ardour_ui_configuration_h__
#define __ardour_ui_configuration_h__

#include <sstream>
#include <ostream>
#include <iostream>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "pbd/stateful.h"
#include "pbd/xml++.h"
#include "ardour/configuration_variable.h"

#include "canvas/colors.h"

#include "utils.h"

class UIConfiguration : public PBD::Stateful
{
    public:
	struct RelativeHSV {
		RelativeHSV (const std::string& b, const ArdourCanvas::HSV& mod) 
			: base_color (b)
			, modifier (mod)
			, quantized_hue (-1.0) {}
		std::string base_color;
		ArdourCanvas::HSV modifier;
		double quantized_hue;

		ArdourCanvas::HSV get() const;
	};

	UIConfiguration();
	~UIConfiguration();

	static UIConfiguration* instance() { return _instance; }

	int load_state ();
	int save_state ();
	int load_defaults ();

	void load_rc_file (bool themechange);
	
	int set_state (const XMLNode&, int version);
	XMLNode& get_state (void);
	XMLNode& get_variables (std::string);
	void set_variables (const XMLNode&);

	typedef std::map<std::string,RelativeHSV> RelativeColors;
	typedef std::map<std::string,std::string> ColorAliases;
	typedef std::map<std::string,ArdourCanvas::Color> BaseColors;

	BaseColors     base_colors;
	RelativeColors relative_colors;
	ColorAliases   color_aliases;

	void set_alias (std::string const & name, std::string const & alias);
	void set_relative (const std::string& name, const RelativeHSV& new_value);
	void set_base (const std::string& name, ArdourCanvas::Color);
	
	RelativeHSV color_as_relative_hsv (ArdourCanvas::Color c);
	std::string color_as_alias (ArdourCanvas::Color c);
	ArdourCanvas::Color quantized (ArdourCanvas::Color) const;

	ArdourCanvas::Color base_color_by_name (const std::string&) const;
	ArdourCanvas::Color color (const std::string&) const;
	ArdourCanvas::HSV  color_hsv (const std::string&) const;

        sigc::signal<void,std::string> ParameterChanged;
	void map_parameters (boost::function<void (std::string)>&);

	void parameter_changed (std::string);
	
#undef UI_CONFIG_VARIABLE
#define UI_CONFIG_VARIABLE(Type,var,name,value) \
	Type get_##var () const { return var.get(); } \
	bool set_##var (Type val) { bool ret = var.set (val); if (ret) { ParameterChanged (name); } return ret;  }
#include "ui_config_vars.h"
#undef  UI_CONFIG_VARIABLE
#define CANVAS_FONT_VARIABLE(var,name) \
	Pango::FontDescription get_##var () const { return ARDOUR_UI_UTILS::sanitized_font (var.get()); } \
	bool set_##var (const std::string& val) { bool ret = var.set (val); if (ret) { ParameterChanged (name); } return ret;  }
#include "canvas_vars.h"
#undef CANVAS_FONT_VARIABLE

#undef CANVAS_BASE_COLOR
#define CANVAS_BASE_COLOR(var,name,val) \
	ArdourCanvas::Color get_##var() const { return base_color_by_name (name); }
#include "base_colors.h"
#undef CANVAS_BASE_COLOR

#undef COLOR_ALIAS
#define COLOR_ALIAS(var,name,alias) ArdourCanvas::Color get_##var() const { return color (name); }
#include "color_aliases.h"
#undef COLOR_ALIAS

  private:
	/* declare variables */

#undef  UI_CONFIG_VARIABLE
#define UI_CONFIG_VARIABLE(Type,var,name,value) ARDOUR::ConfigVariable<Type> var;
#include "ui_config_vars.h"
#undef UI_CONFIG_VARIABLE

#define CANVAS_FONT_VARIABLE(var,name) ARDOUR::ConfigVariable<std::string> var;
#include "canvas_vars.h"
#undef CANVAS_FONT_VARIABLE

	XMLNode& state ();
	bool _dirty;
	bool base_modified;
	bool aliases_modified;
	bool derived_modified;
	
	static UIConfiguration* _instance;

	int store_color_theme (std::string const &);
	void load_base_colors (XMLNode const &);
	void load_color_aliases (XMLNode const &);
	void load_relative_colors (XMLNode const &);
	void reset_gtk_theme ();
	void colors_changed ();
	int load_color_theme ();
};

std::ostream& operator<< (std::ostream& o, const UIConfiguration::RelativeHSV& rhsv);

#endif /* __ardour_ui_configuration_h__ */

