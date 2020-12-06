# Define a class that wraps a system test where system represents 
# either competitors or Figaro method. 
# paths: log,  - build logsalongside output 
#        dump, - dump data 
#        comp/perf - performance comparison
#        comp/prec - precision comparsion
# 

from enum import IntEnum, auto
from abc import ABC, abstractmethod
import abc
import subprocess
import logging
import os
import json
import argparse
import typing
from  collections import deque
from data_management.database import Database
from data_management.database_psql import DatabasePsql
from data_management.database_psql import JOIN_TABLE_NAME

# Class that wraps performance parameters used in testing
class PerformanceConf:
    def __init__(self, path: str):
        self.path = path


# Class that wraps precisions elements 
class PrecisionConf:
    def __init__(self, path: str):
        self.path = path


class SystemTest(ABC):
    # Log test is only for debugging 
    # Dump is used for performance where data is later compared
    # PerformanceConf evaluates speed of the algorithm 
    # PrecisionConf compares data from dumps 
    class TestMode(IntEnum): 
        DEBUG = 1
        DUMP = 2
        PERFORMANCE = 3
        PRECISION = 4

    map_mode_to_str = {TestMode.DEBUG : "DEBUG", TestMode.DUMP: "DUMP", 
                    TestMode.PERFORMANCE: "PERFORMANCE", TestMode.PRECISION: "PRECISION"}

    @staticmethod
    def test_mode_to_str(test_mode: TestMode)->str: 
        return SystemTest.map_mode_to_str[test_mode]


    def __init__(self, name, path_log: str, path_dump: str, 
    perf_conf: PerformanceConf, prec_conf: PrecisionConf, database: Database,
    test_mode = TestMode.PERFORMANCE):
        self.name = name
        self.prec = prec_conf
        self.perf = perf_conf
        self.path_log = path_log
        self.path_dump = path_dump
        self.database = database
        self.test_mode = test_mode
        self.system_test_paper = None


    @staticmethod 
    def create_abs_path(path: str): 
        non_existing_paths = deque()
        cur_path = path 
        
        while not os.path.exists(cur_path):
            non_existing_paths.appendleft(cur_path)
            cur_path, _ = os.path.split(cur_path)

        for non_existing_path in non_existing_paths:
            os.makedirs(non_existing_path)


    @staticmethod
    def create_dir_with_name(path: str, dir_name: str)-> str:
        dir_abs_path = os.path.join(path, dir_name)
        SystemTest.create_abs_path(dir_abs_path)

        return dir_abs_path


    @classmethod
    def from_specs_path(cls, system_test_specs_path: str,
        database_specs_path: str, test_mode: TestMode, *args, **kwargs):
        database = None
        with open(system_test_specs_path) as json_file:
            system_json = json.load(json_file)
            
        database = Database(database_specs_path)
        path_log = SystemTest.create_dir_with_name(
            system_json["system"]["log"]["path"], database.name)
        path_dump = SystemTest.create_dir_with_name(
            system_json["system"]["dump"]["path"], database.name)
        return cls(path_log, path_dump, PrecisionConf(""), 
                PerformanceConf(""), database, 
                test_mode, *args, **kwargs)


    def run(self):
        logging.info("Starting of test {}".format(self.name))
        if self.test_mode == SystemTest.TestMode.DEBUG:
            logging.info("Run debug")
            self.run_debug()
        elif self.test_mode == SystemTest.TestMode.DUMP:
            logging.info("Run dump")
            self.run_dump()
        elif self.test_mode == SystemTest.TestMode.PRECISION:
            logging.info("Run precision")
            self.run_precision()
        elif self.test_mode == SystemTest.TestMode.PERFORMANCE:
            logging.info("Run performance")
            self.run_performance()
        else:
            logging.error('This type of system test does not exist')
        logging.info("End of test {}".format(self.name))
    
    @abstractmethod
    def run_debug(self):
        pass


    @abstractmethod
    def run_dump(self):
        pass


    @abstractmethod
    def run_precision(self):
        pass

    
    @abstractmethod
    def run_performance(self):
        pass

    
    def is_dbms(self):
        return False

    
    def requires_dbms_result(self):
        return False


    def is_paper_algorithm(self):
        return False


    def set_paper_system_test(self, system_test_paper):
        self.system_test_paper = system_test_paper
    
    # Deletes all the auxilary data from the correpsonding path
    def clean_data(self, test_data_type: TestMode):
        pass

    def clean_all_data(self):
        pass
        #for test_data_type in TestMode:
        #    self.clean_data(test_data_type)
