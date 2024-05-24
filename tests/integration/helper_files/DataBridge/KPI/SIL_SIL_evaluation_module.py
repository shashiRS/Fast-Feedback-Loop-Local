import csv
import ntpath
import os
import threading
import time

kExportFileName = "SIL_SIL_evaluation_kpi.csv"


class csv_parser:
    def __init__(self, f_name, num_thrd, ref_file=""):
        self.folder_name_ = f_name.replace(" ", "")
        self.number_of_threads_ = num_thrd
        self.exported_data_ = {}
        self.lock_ = threading.Lock()
        self.evaluation_result_ = list()
        self.evaluation_file_ = ""
        self.reference_file_ = ref_file.replace(" ", "")

    def start_kpi(self):
        self.start_parsing_threads()
        self.evaluation()

    def get_list_of_files(self, folder_name):
        self.evaluation_file_ = os.path.abspath(os.path.join(self.folder_name_, kExportFileName))
        if os.path.exists(self.evaluation_file_):
            os.remove(self.evaluation_file_)
        files = []
        for dirpath, _, filenames in os.walk(folder_name):
            for f in filenames:
                files.append(os.path.abspath(os.path.join(dirpath, f)))
        return files

    def parse_csv_files(self, file):
        print("Start parsing file:  ", file)
        with open(file, newline="") as csvfile:
            csv_contain = csv.reader(csvfile, delimiter=",", quotechar="|")
            head, tail = ntpath.split(file)
            csv_file_parsed = []
            for row in csv_contain:
                try:
                    csv_file_parsed.append([float(x) if x.isdigit() else 0 for x in row])
                except:  # noqa E722 TODO need to check if file is needed at all. If so add proper except
                    print("Parsing CSV file error")
                    pass

            with self.lock_:
                self.exported_data_[ntpath.basename(tail)] = csv_file_parsed

    def start_parsing_threads(self):
        files = self.get_list_of_files(self.folder_name_)

        threads = list()
        for file in files:
            while len(threads) >= self.number_of_threads_:
                for indx, thrd in enumerate(threads):
                    if not thrd.is_alive():
                        del threads[indx]
                time.sleep(0.1)

            thrd = threading.Thread(target=self.parse_csv_files, args=(file,))
            threads.append(thrd)
            thrd.start()

        for index, thread in enumerate(threads):
            thread.join()

        threads.clear()

        print("Parsing file threads are finished!!")

    def evaluation_sub_job(self, ref_file, other_file, f_name):
        smallest_length = len(ref_file) if len(ref_file) < len(other_file) else len(other_file)
        cmp_res = [None] * smallest_length
        cmp_res[0] = f_name
        for ind in range(smallest_length):
            if ref_file[ind] == other_file[ind]:
                cmp_res[ind] = 0
            else:
                found = False
                for indx, val in enumerate(other_file):
                    if ref_file[ind] == val:
                        if indx < ind:
                            cmp_res[ind] = -indx
                        else:
                            cmp_res[ind] = indx
                        found = True
                        break
                if not found:
                    cmp_res[ind] = "E"

        with self.lock_:
            self.evaluation_result_.append(cmp_res)

    def evaluation(self):
        threads = list()
        keys = list(self.exported_data_.keys())
        if len(keys) >= 2:
            ref_file = ""
            start_ind = 0

            # Check for reference file
            if self.reference_file_ and os.path.exists(
                os.path.abspath(os.path.join(self.folder_name_, self.reference_file_))
            ):
                ref_file = self.exported_data_[self.reference_file_]
                keys.remove(self.reference_file_)
            else:
                ref_file = self.exported_data_[keys[0]]
                start_ind = 1

            for key in keys[start_ind:]:
                while len(threads) >= self.number_of_threads_:
                    for indx, thrd in enumerate(threads):
                        if not thrd.is_alive():
                            del threads[indx]
                    time.sleep(0.1)

                print("Start comparison with the reference file: ", key)
                thrd = threading.Thread(target=self.evaluation_sub_job, args=(ref_file, self.exported_data_[key], key))
                threads.append(thrd)
                thrd.start()

            for index, thread in enumerate(threads):
                thread.join()

        else:
            print("At least two exported files are needed!!!")

        with open(self.evaluation_file_, "w", newline="") as file:
            writer = csv.writer(file, delimiter=",", dialect="excel-tab")
            transpose_list = [list(i) for i in zip(*self.evaluation_result_)]
            writer.writerows(transpose_list)

        print("Evaluation finished!!!")
