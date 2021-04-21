from enum import Enum, auto
import subprocess
import os
import json
import logging
from data_management.database import Database
from evaluation.system_test import DumpConf, LogConf, QueryConf, SystemTest
from evaluation.system_test import AccuracyConf
from evaluation.system_test import PerformanceConf
from evaluation.system_test_competitor import SystemTestCompetitor

class SystemTestPython(SystemTestCompetitor):
    def __init__(self, log_conf: LogConf, dump_conf: DumpConf,
    perf_conf: PerformanceConf, accur_conf: AccuracyConf, database: Database,
    query_conf: QueryConf,
    test_mode = SystemTest.TestMode.PERFORMANCE, **kwargs):
        super().__init__("PYTHON", log_conf, dump_conf, perf_conf, accur_conf, database, query_conf, test_mode)


    def set_join_result_path(self, join_path):
        self.join_path = join_path


    def run_debug(self):
        pass


    def eval(self, dump: bool, performance: bool):
        args = ["python3",
            "/home/popina/Figaro/figaro-code/competitors/python/qr.py",
            "--data_path", self.join_path]
        if dump:
            args += [ "--dump_file", self.conf_dump.file_path,
                     "--precision", str(self.conf_accur.precision)]

        if performance:
            args += ["--num_repetitions", str(self.conf_perf.num_reps)]

        result = subprocess.run(args=args,  capture_output=True, text=True)
        path_log_file = self.conf_log.file_path
        with open(path_log_file, "w") as log_file:
            log_file.write(result.stdout)
        logging.error(result.stderr)


    def run_dump(self):
        self.eval(dump=True, performance=False)


    def run_performance(self):
        self.eval(dump=False, performance=True)


    def requires_dbms_result(self):
        return True
