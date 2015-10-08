
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"

#include "KLT/DLX/compiler.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <locale>

void split(const std::string & str, char delim, std::vector<std::string> & elems) {
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delim))
        elems.push_back(item);
}

int main(int argc, char ** argv) {
  std::vector<std::string> args;

  ::KLT::Descriptor::target_kind_e threads_target = ::KLT::Descriptor::e_target_unknown;
  ::KLT::Descriptor::target_kind_e accelerator_target = ::KLT::Descriptor::e_target_unknown;

  std::string target_option_str("--tilek-target=");
  std::string threads_target_str("threads");
  std::string opencl_target_str("opencl");
  std::string cuda_target_str("cuda");
  args.push_back(argv[0]);
  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg.find(target_option_str) == 0) {
      std::vector<std::string> targets;
      split(arg.substr(target_option_str.length()), ',', targets);
      std::vector<std::string>::iterator it_target;
      for (it_target = targets.begin(); it_target != targets.end(); it_target++) {
        std::transform(it_target->begin(), it_target->end(), it_target->begin(), ::tolower);
        if (it_target->find(threads_target_str) == 0) {
          assert(it_target->length() == threads_target_str.length());
          assert(threads_target == ::KLT::Descriptor::e_target_unknown);
          threads_target = ::KLT::Descriptor::e_target_threads;
        }
        else if (it_target->find(opencl_target_str) == 0) {
          assert(it_target->length() == opencl_target_str.length());
          assert(accelerator_target == ::KLT::Descriptor::e_target_unknown);
          accelerator_target = ::KLT::Descriptor::e_target_opencl;
        }
        else if (it_target->find(cuda_target_str) == 0) {
          assert(it_target->length() == cuda_target_str.length());
          assert(accelerator_target == ::KLT::Descriptor::e_target_unknown);
          accelerator_target = ::KLT::Descriptor::e_target_cuda;
        }
        else {
          assert(false);
        }
      }
    }
    else
      args.push_back(arg);
  }

  args.push_back("-DSKIP_OPENCL_SPECIFIC_DEFINITION");

  SgProject * project = new SgProject(args);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

  KLT::DLX::Compiler< ::DLX::TileK::language_t> compiler(project, KLT_PATH, basename, threads_target, accelerator_target);

//  MFB::api_t * api = compiler.getDriver().getAPI();
//  dump_api(api);

  compiler.compile(project);

  project->unparse();

  return 0;
}

