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
- Then run: ./repro.sh and follow the instructions as the script asks:
    1. The script will create a new docker container with docker local paths pointing to DUMP_PATH, DATA_PATH and POSTGRES_PATH (specified in the previous three variables).
    2. The script will then create a new root in the docker container: zivanovic and the password specified on the prompt (PASSWORD).
    3. Then the libraries will be installed in the docker container using library_setup.sh. At certain points the installation will require PASSWORD.
    4. Experiments will be run as a script run_experiments.sh:
        1. Cloning reproducibility repository to the docker container.
        2. Setting up environments.
        3. Downloading the data.
        4. Evaluation of experiments alongside collecting the results and plotting the data as in the paper. The results will be located in the following path in the docker:
        ```/home/zivanovic/Figaro/figaro-code/scripts/results```
        and in the following path: 
        ```./plots``` on the local machine. 
        Non-ohe and ohe inside .dat files and .pdf files.
        Synthetic: inside .tex files.
