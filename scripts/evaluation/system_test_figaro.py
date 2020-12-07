
from enum import Enum, auto
import subprocess
import os
import json
import logging
from data_management.database import Database
from evaluation.system_test import SystemTest
from evaluation.system_test import SystemTest
from evaluation.system_test import AccuracyConf
from evaluation.system_test import PerformanceConf

class SystemTestFigaro(SystemTest):
    def __init__(self, path_log: str, path_dump: str, 
            perf_conf: PerformanceConf, accur_conf: AccuracyConf, database: Database,
            test_mode = SystemTest.TestMode.PERFORMANCE, *args, **kwargs):
        super().__init__("FIGARO", path_log, path_dump, perf_conf, 
            accur_conf, database, test_mode)


    def run_debug(self):
        pass


    def run_dump(self):
        test_mode = SystemTest.test_mode_to_str(self.test_mode)
        logging.error(test_mode)
        args = ["/bin/bash", "setup.sh", 
                "--log_path={}".format(self.path_log),
                "--dump_path={}".format(self.path_dump),
                "--precision={}".format(self.conf_accur.precision),
                "--test_mode={}".format
                (SystemTest.test_mode_to_str(self.test_mode))]
        result = subprocess.run(
            args=args, cwd="/home/popina/Figaro/figaro-code/figaro", 
            capture_output=True, text=True, shell=False)
        logging.info(result.stdout)
        logging.error(result.stderr)


    def run_accuracy(self):
        pass

    
    def run_performance(self):
        pass


    def is_paper_algorithm(self):
        return True
        

    def is_dbms(self):
        return False

    
    def requires_dbms_result(self):
        return False