#include "database/Database.h"
#include "database/query/Query.h"
#include "database/query/visitor/ASTVisitorQRResult.h"
#include "utils/Performance.h"

#include <boost/program_options.hpp>
#include <fstream>
#include <omp.h>
#include "utils/Utils.h"

namespace po = boost::program_options;

void initGlobalState(uint32_t numThreads = 48)
{
    omp_set_nested(1);
    omp_set_dynamic(0);
    omp_set_num_threads(numThreads);
    FIGARO_LOG_INFO("Number of threads is", numThreads)
}

int main(int argc, char *argv[])
{
    std::string implementation;
    std::string dumpFilePath;
    std::string dbConfigPath;
    std::string postprocessMode;
    std::string strMemoryLayout;
    std::string queryConfigPath;

    Figaro::QRGivensHintType qrHintType = Figaro::QRGivensHintType::THIN_DIAG;
    Figaro::MemoryLayout memoryLayout = Figaro::MemoryLayout::ROW_MAJOR;
    bool dump = false;
    bool pureFigaro = false;
    uint32_t precision;
    uint32_t numRepetitions = 1;
    uint32_t numThreads = 1;

    std::string strArgs = "";
    for (uint32_t idx = 0; idx < argc; idx++)
    {
        strArgs += argv[idx] + std::string(" ");
    }
    FIGARO_LOG_INFO("Command line args", strArgs)


    po::options_description desc("figaro - allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("implementation", po::value<std::string>(&implementation))
    ("dump_file_path", po::value<std::string>(&dumpFilePath))
    ("db_config_path", po::value<std::string>(&dbConfigPath))
    ("query_config_path", po::value<std::string>(&queryConfigPath))
    ("precision", po::value<uint32_t>(&precision))
    ("num_repetitions", po::value<uint32_t>(&numRepetitions))
    ("num_threads", po::value<uint32_t>(&numThreads))
    ("postprocess", po::value<std::string>(&postprocessMode))
    ("memory_layout", po::value<std::string>(&strMemoryLayout))
    ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    if (vm.count("dump_file_path"))
    {
        dumpFilePath = vm["dump_file_path"].as<std::string>();
        dump = true;
        FIGARO_LOG_INFO(dumpFilePath)
    }

    if (vm.count("num_repetitions"))
    {
        numRepetitions = vm["num_repetitions"].as<std::uint32_t>();
    }

    if (vm.count("num_threads"))
    {
        numThreads = vm["num_threads"].as<std::uint32_t>();
    }

    if (vm.count("postprocess"))
    {
        postprocessMode = vm["postprocess"].as<std::string>();
        if (postprocessMode == "THIN_BOTTOM")
        {
            qrHintType = Figaro::QRGivensHintType::THIN_BOTTOM;
        }
        else if (postprocessMode == "THIN_DIAG")
        {
            qrHintType = Figaro::QRGivensHintType::THIN_DIAG;
        }
        else if (postprocessMode == "THICK_BOTTOM")
        {
            qrHintType = Figaro::QRGivensHintType::THICK_BOTTOM;
        }
        else if (postprocessMode == "THICK_DIAG")
        {
            qrHintType = Figaro::QRGivensHintType::THICK_DIAG;
        }
        else if (postprocessMode == "LAPACK")
        {
            qrHintType = Figaro::QRGivensHintType::LAPACK;
        }
        FIGARO_LOG_INFO("postprocessMode", postprocessMode)
    }

    if (vm.count("memory_layout"))
    {
        strMemoryLayout = vm["memory_layout"].as<std::string>();
        if (strMemoryLayout == "ROW_MAJOR")
        {
            memoryLayout = Figaro::MemoryLayout::ROW_MAJOR;
        }
        else if (strMemoryLayout == "COL_MAJOR")
        {
            memoryLayout = Figaro::MemoryLayout::COL_MAJOR;
        }
    }

    if (vm.count("implementation"))
    {
        implementation = vm["implementation"].as<std::string>();
        if (implementation == "figaro")
        {
            pureFigaro = true;
        }
        else if (implementation == "postprocess")
        {
            pureFigaro = false;
        }
    }

    dbConfigPath = vm["db_config_path"].as<std::string>();
    queryConfigPath = vm["query_config_path"].as<std::string>();
    FIGARO_LOG_INFO(dbConfigPath)


    initGlobalState(numThreads);

    Figaro::Database database(dbConfigPath);
    database.loadData();

    Figaro::Query query(&database);
    query.loadQuery(queryConfigPath);
    query.evaluateQuery(true, true, true, true, qrHintType, memoryLayout, dump);
    FIGARO_LOG_INFO("WTF")
    if (dump)
    {
        Figaro::ASTVisitorAbsResult* pResult = query.getResult();
        FIGARO_LOG_INFO("Dumping R to the path", dumpFilePath);
        FIGARO_LOG_BENCH("dumping", "hohl")
        if (pResult->getResultType() == Figaro::ASTVisitorAbsResult::ResultType::QR_RESULT)
        {
            FIGARO_LOG_BENCH("dumping", "QR")
            Figaro::ASTVisitorQRResult* pQrResult = (Figaro::ASTVisitorQRResult*)pResult;
            std::ofstream fileDumpR(dumpFilePath, std::ofstream::out);
            database.outputRelationToFile(fileDumpR,
                pQrResult->getRRelationName(), ',', precision);
        }
        else if (pResult->getResultType() == Figaro::ASTVisitorAbsResult::ResultType::JOIN_RESULT)
        {
            FIGARO_LOG_BENCH("dumping", "JOIN")

        }
    }

    FIGARO_LOG_INFO("Figaro program has terminated")
    return 0;
}