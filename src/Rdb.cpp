#include "../include/config.hpp"
#include "../include/RdbParser.hpp"

int test_rdb_main(int argc, char* argv[]) {
    try {
        Config config = parse_args(argc, argv);
        std::filesystem::path target_rdb = config.get_rdb_path();

        if (std::filesystem::exists(target_rdb)) {
            RdbParser parser(target_rdb);
            parser.parse_header();
            
            auto data_store = parser.parse_db_entries();
            std::cout << "Parsed " << data_store.size() << " keys.\n";
        } else {
            std::cout << "No snapshot base found at: " << target_rdb << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}