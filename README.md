The code and data provided in our repository are to be used exclusively for the purpose of reproducibility. They are not to be used in a different setting, for academic papers or industry setting without our explicit consent. Intruction details follow.\
Required OS: Linux.\
Required apps: Docker, bash.\
Required machine: at least 500 GB of free disk space, 192GiB of RAM, 24 physical/48 logical cores.

- Clone a repository from the account with the abovementioned details. Checkout to the branch Sigmod-2022-Repro that contains the code used in the reproducibility submission.
- Move all the scripts from figaro-code/reproducibility to the current folder.
- Change permissions for the scripts to executable (chmod 777).
- Update DOCKER_DATA_PATH, DOCKER_DUMP_PATH, DOCKER_POSTGRES_PATH in the repro.sh script so they point to the appropriate paths on the local machine. These paths should have 500GB of free storage in total.
- Then run: ./repro.sh and follow the instructions as the script asks:
    1. The script will create a new docker container with mapping DUMP_PATH, DATA_PATH and POSTGRES_PATH to the appropriate local paths (specified in the previous three variables).
    2. The script will then create a new root in the docker container: zivanovic and the password specified by the user using user_install.sh.
    3. Then the libraries will be installed in the docker container using library_setup.sh. At certain points the installation will require root password (set by the user).
    4. Experiments will be run as a script run_experiments.sh:
        1. Cloning reproducibility repository to the docker container. (it requires username and password of the repository).
        2. Setting up environments.
        3. Downloading the data.
        4. Evaluation of experiments alongside collecting the results and plotting the data as in the paper. The results will be located in the following path in the docker:
        /home/zivanovic/Figaro/figaro-code/scripts/results
        Non-ohe and ohe inside .dat files and .pdf files.
        Synthetic: inside .tex files.
