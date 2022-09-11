The code and data provided in our repository are to be used exclusively for the purpose of reproducibility. They are not to be used in a different setting, for academic papers or industry setting without our explicit consent. Intruction details follow.\
Required OS: Linux.\
Required apps: Docker, bash.\
Required machine: at least 500 GiB of free disk space, 192GiB of RAM, 24 physical/48 logical cores. Not all experiments require this. Checkout [Figaro](figaro-code/README.md) for more.

- Download file figaro-code/reproducibility/repro.sh to your machine. 
- Update DOCKER_DATA_PATH, DOCKER_DUMP_PATH, DOCKER_POSTGRES_PATH in the repro.sh script so they point to the appropriate paths on the local machine. These paths should have 500GB of free storage in total. For example (this is on our machine):
```
DOCKER_DATA_PATH=/local/scratch/Sigmod2022Repro/data/
DOCKER_DUMP_PATH=/local/scratch/Sigmod2022Repro/dumps/
DOCKER_POSTGRESS_PATH=/local/scratch/Sigmod2022Repro/postgresData/
```
 where /local/scratch/Sigmod2022Repro has sufficient disk space. 
- Then run: ./repro.sh:
    1. The script will create a new docker container with docker local paths pointing to DUMP_PATH, DATA_PATH and POSTGRES_PATH (specified in the previous three variables).
    2. The script will then create a new root in the docker container: zivanovic.
    3. Then the libraries will be installed in the docker container using library_setup.sh.
    4. Experiments will be run as a script run_experiments.sh:
        1. Cloning reproducibility repository to the docker container.
        2. Setting up environments.
        3. Downloading the data.
        4. Evaluation of experiments alongside collecting the results and plotting the data as in the paper. The results will be located in the following path in the docker:
        ```/home/zivanovic/Figaro/figaro-code/scripts/results```
        and in the following path: 
        ```./plots``` on the local machine.
            1. Results for real datasets are in non-ohe subdirectory. exp1perf.pdf contains Figure 4 top. The corresponding times are in exp1perf-DATASE_NAME.dat. exp2threads.png contains Figure 6 where y axis shows times instead of speed ups. exp2cores.dat contains times for this experiment.\
            exp3perf-DATASET_NAME.dat contains runtimes for different join orders used in experiment 3 where DATASET_NAME refers to the dataset name. RELATION_NAMERoot is used to name the corresponding join order where RELATION_NAME represents a root relation. For example, InvetoryRoot represent a join order where the relation Inventory is a root node (the same as in the paper I(L(C), W, T). Then, the speed up between the slowest and fastest can be checked manually.\
            **Note**. The experiment 2 is ran for LocationRoot join order for Retailer, for StoresRoot join order for Favorita and BusinessRoot for Yelp. However, the script generates exp2cores.dat for the fastest join orders, and due to close runtimes of some orders (for example on our machine: BusinessRoot and HoursRoot 0.44s and 0.45s, respecitvely), the script can choose a wrong join order for the experiment 2. In that case, the times should be read from exp3perf-DATASET_NAME.dat. 
            2. Results for OHE real datasets will be in a subdirectory ohe. The same file organization as with non-ohe datasets except there is no experiment 2. 
            3. Synthetic: synthetic_perf.tex: Figure 5. synthetic_accur.tex: Table 3.  
