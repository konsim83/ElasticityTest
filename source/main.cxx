#include "run_problem.h"

int
main(int argc, char *argv[])
{
  // Very simple way of input handling.
  if (argc < 2)
    {
      std::cout << "You must provide an input file \"-p <filename>\""
                << std::endl;
      exit(1);
    }

  std::string input_file = "";

  std::list<std::string> args;
  for (int i = 1; i < argc; ++i)
    {
      args.push_back(argv[i]);
    }

  while (args.size())
    {
      if (args.front() == std::string("-p"))
        {
          if (args.size() == 1) /* This is not robust. */
            {
              std::cerr << "Error: flag '-p' must be followed by the "
                        << "name of a parameter file." << std::endl;
              exit(1);
            }
          else
            {
              args.pop_front();
              input_file = args.front();
              args.pop_front();
            }
        }
      else
        {
          std::cerr << "Unknown command line option: " << args.front()
                    << std::endl;
          exit(1);
        }
    } // end while
  try
    {
      using namespace dealii;
      using namespace Elasticity;

      deallog.depth_console(2);

      // For debugging or if we use PetSc it may be nice to limit the threads on
      // each process.
#if DEBUG
      dealii::MultithreadInfo::set_thread_limit(1);
      dealii::Utilities::MPI::MPI_InitFinalize mpi_initialization(
        argc, argv, /* max_threads */ 1);

#else
      dealii::Utilities::MPI::MPI_InitFinalize mpi_initialization(
        argc, argv, dealii::numbers::invalid_unsigned_int);
#endif

      const bool say_hello_from_cluster = true;
      // #if DEBUG
      //         true;
      // #else
      //         false;
      // #endif

      if (say_hello_from_cluster)
        {
          char processor_name[MPI_MAX_PROCESSOR_NAME];
          int  name_len;
          MPI_Get_processor_name(processor_name, &name_len);

          std::string proc_name(processor_name, name_len);

          std::cout << "Hello from   " << proc_name << "   Rank:   "
                    << dealii::Utilities::MPI::this_mpi_process(MPI_COMM_WORLD)
                    << "   out of   "
                    << dealii::Utilities::MPI::n_mpi_processes(MPI_COMM_WORLD)
                    << "   | cores = " << dealii::MultithreadInfo::n_cores()
                    << "   | threads = " << dealii::MultithreadInfo::n_threads()
                    << std::endl;
        }

      const Dimension dimension(input_file);
      const int       dim = dimension.dim;

      switch (dim)
        {
          case 2:
            run_2d_problem(input_file);
            break;

          case 3:
            run_3d_problem(input_file);
            break;

          default:
            std::cout << "The dimension must be 2 or 3." << std::endl;
            exit(1);
        }
    }
  catch (std::exception &exc)
    {
      std::cerr << std::endl
                << std::endl
                << "----------------------------------------------------"
                << std::endl;
      std::cerr << "Exception on processing: " << std::endl
                << exc.what() << std::endl
                << "Aborting!" << std::endl
                << "----------------------------------------------------"
                << std::endl;
      return 1;
    }
  catch (...)
    {
      std::cerr << std::endl
                << std::endl
                << "----------------------------------------------------"
                << std::endl;
      std::cerr << "Unknown exception!" << std::endl
                << "Aborting!" << std::endl
                << "----------------------------------------------------"
                << std::endl;
      return 1;
    }
  return 0;
}