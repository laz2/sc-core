/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2011 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

/// @brief Semantic Code Compiler
/// @author Dmitry Lazurkin
///

#include <boost/scoped_ptr.hpp>

#include <tclap/CmdLine.h>

#include <libsc/tgf_io.h>

#include <libtgf/tgfout.h>

#include <libscc/libscc.h>

TCLAP::UnlabeledValueArg<std::string> arg_input_file("input-file", "Input source file", true, "", "input file");
TCLAP::ValueArg<std::string> arg_output_file("o", "output", "Generated output file", false, "", "path to file");
TCLAP::MultiArg<std::string> arg_include_dir("I", "include-dir", "Include directory path", false, "path to directory");

TCLAP::SwitchArg arg_debug("d", "debug", "Turn on debug output", false);
TCLAP::SwitchArg arg_new_sc_alpha("a", "new-sc-alpha", "Turn on new sc-alphabet usage.", false);

void save_tgf(const fs::path &output_path, sc_segment *freeseg)
{
	FILE *output = fopen(output_path.string().c_str(), "wb");
	int fd = fileno(output);
	tgf_stream_out *tgf_out = tgf_stream_out_new();

	tgf_stream_out_fd(tgf_out, fd);

	if (tgf_stream_out_start(tgf_out, "SCC") < 0) {
		std::cerr << "TGF output error: " << tgf_error(tgf_stream_out_get_error(tgf_out)) << std::endl;
		return;
	}

	write_to(system_session, freeseg, tgf_out);

	if (tgf_stream_out_finish(tgf_out) < 0) {
		std::cerr << "TGF output error: " << tgf_error(tgf_stream_out_get_error(tgf_out)) << std::endl;
		return;
	}

	tgf_stream_out_destroy(tgf_out);
	//fclose(output);
}

int main(int argc, char *argv[])
{
	try {
		TCLAP::CmdLine cmd("Semantic Code Compiler", ' ', "0.1");

		cmd.add(arg_output_file);
		cmd.add(arg_include_dir);
		cmd.add(arg_input_file);
		cmd.add(arg_debug);
		cmd.add(arg_new_sc_alpha);

		cmd.parse(argc, argv);
	} catch (TCLAP::ArgException &e) {
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
		return 1;
	}

	bool from_stdin = false;
	fs::path input_path;

	// Check input source file.
	//
	sc_string input  = arg_input_file.getValue();
	if (input != "-") {
		input_path  = fs::absolute(input);
		if (!fs::exists(input_path) || !fs::is_regular_file(input_path)) {
			std::cerr << "File for path " << input_path << " not found" << std::endl;
			return 2;
		}

		// Check extension of input file.
		//
		fs::path input_ext = input_path.extension();

		if (input_ext == ".scsy") {
			std::cerr << input_path << ": file has extension .scsy, but it is reserved for include scs sources."
				<< "Cannot process this file." << std::endl;
			return 3;
		}

		if (input_ext != ".scs" && input_ext != ".scps") {
			std::cerr << input_path << ": file has unknown extension \"" << input_ext
				<< "\". Known how to process only sources with extensions \".scs\" and \".scps\"." << std::endl;
			return 4;
		}
	} else {
		from_stdin = true;
	}

	// Build path for output file.
	//
	fs::path output_path;
	if (arg_output_file.isSet()) {
		output_path = fs::absolute(arg_output_file.getValue());
	} else {
		if (from_stdin) {
			std::cerr << "Input stream is stdin, please specify output file." << std::endl;
			return 5;
		}

		output_path = input_path.parent_path() / input_path.stem();
	}

	if (fs::is_directory(output_path)) {
		std::cerr << "Error: Output path " << output_path << " is path of existent directory." << std::endl;
		return 5;
	}

	sc_segment *seg = system_session->create_segment("/tmp/scc");
	scc::cc = new scc::compiler(system_session, seg);
	boost::scoped_ptr<scc::compiler> compiler_deleter(scc::cc);

	scc::cc->debug_mode(arg_debug.getValue());
	scc::cc->new_sc_aplha(arg_new_sc_alpha.getValue());
	scc::cc->set_cur_dir(input_path.parent_path());

	//
	// Add include directories
	//
	scc::cc->add_include_dir(scc::cc->get_cur_dir());
	const std::vector<std::string> &include_dirs = arg_include_dir.getValue();
	for (size_t i = 0; i < include_dirs.size(); ++i)
		scc::cc->add_include_dir(fs::path(include_dirs[i]));

	try {
		if (from_stdin)
			scc::cc->compile(input_path);
		else
			scc::cc->compile();
	} catch (scc::lang_error &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 5;
	}

	save_tgf(output_path, seg);
	return 0;
}
