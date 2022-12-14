source ~/.non_inter_paths.sh
FIGARO_PATH=/home/zivanovic/Figaro
FIGARO_PSQL_PATH=/figaroPostgresql
FIGARO_DUMP_PATH=/figaroDumps
FIGARO_DATA_PATH=/figaroData
mkdir -p ${FIGARO_PATH}

#cd $FIGARO_DATA_PATH
#rm * -rf
#sudo chown zivanovic $FIGARO_DATA_PATH
cd $FIGARO_DUMP_PATH
rm * -rf
sudo chown zivanovic $FIGARO_DUMP_PATH

ln -s $FIGARO_DATA_PATH ${FIGARO_PATH}/data

############## Path setup ################
FIGARO_CODE_PATH=$FIGARO_PATH/figaro/figaro-code
FIGARO_DATA_PATH=$FIGARO_PATH/data
FIGARO_SYSTEMS_TESTS_PATH=$FIGARO_CODE_PATH/system_tests
FIGARO_SCRIPTS_PATH=$FIGARO_CODE_PATH/scripts
FIGARO_EXP_RES_PATH=$FIGARO_SCRIPTS_PATH/results
FIGARO_PSQL_USER=zivanovic
FIGARO_PSQL_PASSWORD=12345
NUMPY_PATH=/home/zivanovic

############# Cloning repository #########
cd $FIGARO_PATH

sudo rm figaro-code -rf
git clone --branch code-branch https://github.com/Sigmod2022ReproFigaro/figaro.git figaro-code
ln -s $FIGARO_DUMP_PATH ${FIGARO_CODE_PATH}/dumps
cd $FIGARO_SCRIPTS_PATH

######### Setting up python OpenBlas environment ########
python3.8 -m venv run-env-openblas
source run-env-openblas/bin/activate
pip install -r requirements.txt
pip uninstall -y numpy
$NUMPY_PATH/numpy-openblas/setup.py install
deactivate

########### Setting up python MKL environment #########
python3.8 -m venv run-env-mkl
source run-env-mkl/bin/activate
pip install -r requirements.txt
pip uninstall -y numpy
$NUMPY_PATH/numpy-mkl/setup.py install
#deactivate

########## Download data #############
python -m data_management.data_formating -r $FIGARO_CODE_PATH -d $FIGARO_DATA_PATH -s $FIGARO_SYSTEMS_TESTS_PATH --backup
#python -m evaluation.data_generation -u $FIGARO_PSQL_USER -p $FIGARO_PSQL_PASSWORD  -s $FIGARO_SYSTEMS_TESTS_PATH -d $FIGARO_DATA_PATH --data_type #download_real_data
#python -m evaluation.data_generation -u $FIGARO_PSQL_USER -p $FIGARO_PSQL_PASSWORD  -s $FIGARO_SYSTEMS_TESTS_PATH -d $FIGARO_DATA_PATH --data_type all
############# Run experiments ###########
for i in {1..4}
do
	python -m evaluation.experiment -u $FIGARO_PSQL_USER -p $FIGARO_PSQL_PASSWORD -r $FIGARO_CODE_PATH -s $FIGARO_SYSTEMS_TESTS_PATH -e $i
done
cd $FIGARO_EXP_RES_PATH
cd non-ohe
gnuplot ../../gnuplots/exp1perf.plot
gnuplot ../../gnuplots/exp2cores.plot
#cd ../ohe
#gnuplot ../../gnuplots/exp2perf.plot
