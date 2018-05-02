#include <iostream>

#include <boost/program_options.hpp>



int main(int argc, char * argv[])
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
        ("help,h", "produce help message")
        ("config,c", po::value<std::string>(), "provide configuration file");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return 0;
  }

  return 0;
}