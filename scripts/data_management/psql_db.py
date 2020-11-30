import psycopg2
from psycopg2 import sql
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT
import argparse

from data_management.database import Database
from data_management.relation import Relation

class DatabasePsql:
    def __init__(self, host_name, user_name, password, database_name = None):
        self.host_name = host_name
        self.user_name = user_name
        self.password = password
        self.database_name = database_name
        if database_name is not None:
            self.open_connection()
        else:
            self.connection = None

    @staticmethod
    def open_connection_static(host_name, user_name, password, database_name):
        connection = psycopg2.connect(host=host_name, user=user_name,
                        password=password, dbname=database_name)
        connection.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
        return connection


    def open_connection_admin(self):
        connection = DatabasePsql.open_connection_static(host_name='', user_name=self.user_name, 
                            password=self.password, database_name='postgres')
        return connection


    def open_connection(self):
        self.connection = self.open_connection_static(self.host_name, self.user_name, 
                                    self.password, self.database_name)
        return self.connection
    

    def close_connection(self):
        if self.connection is not None:
            self.connection.close()
        self.connection = None


    def create_table(self, relation: Relation):
        attributes = relation.attributes
        
        sql_attributes = ""
        for attribute in attributes:
            sql_attribute = "{} {}" 
            sql_attribute = sql_attribute.format(attribute.name, attribute.type)
            sql_attributes += sql_attribute + ","
            print(sql_attributes)
        
        sql_pks = "PRIMARY KEY({})"
        sql_pk_attributes = ""
        for attribute_name in relation.get_set_pk_attribute_names():
            sql_pk_attributes += attribute_name + ","
        sql_pks = sql_pks.format(sql_pk_attributes[:-1])
        
        sql_attributes += sql_pks
        sql_query = "CREATE TABLE {} ({});"
        sql_query = sql_query.format(relation.name, sql_attributes)
        
        cursor = self.connection.cursor()
        print(sql_query)
        cursor.execute(sql_query)
        cursor.close()

        self.import_data_to_table(relation.name, relation.data_path)


    def import_data_to_table(self, table_name: str, data_path: str):
        cursor = self.connection.cursor()
        with open(data_path) as data_csv:
            cursor.copy_from(data_csv, table_name, sep=',')
        cursor.close()

    
    def drop_table(self, relation):
        pass


    def evaluate_join(self, table_names: list = None):
        sql_join = "CREATE TABLE join_table AS (SELECT * FROM {});"
        sql_from_natural_join = ""
        for idx, table_name in enumerate(table_names):
            sql_from_table_name = table_name if idx == 0 \
                else  " NATURAL JOIN " + table_name
            sql_from_natural_join += sql_from_table_name
        sql_join = sql_join.format(sql_from_natural_join)

        print (sql_join)
        cursor = self.connection.cursor()
        cursor.execute(sql_join)
        cursor.close()



    # Passes database spec to be created, 
    def create_database(self, database: Database):
        self.close_connection()
        self.database_name = database.name

        connection = self.open_connection_admin()
        cursor = connection.cursor()
        cursor.execute(sql.SQL("CREATE DATABASE {}").format(
                            sql.Identifier(self.database_name)))
        cursor.close()
        connection.close()
        self.open_connection()

        relations = database.get_relations()
        for relation in relations:
            self.create_table(relation)



    def drop_database(self):
        self.close_connection()
        connection = self.open_connection_admin()
        cursor = connection.cursor()
        cursor.execute(sql.SQL("DROP DATABASE {}").format(
                            sql.Identifier(self.database_name)))
        cursor.close()
        connection.close()
        self.database_name = None



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--password", action="store",  
                        dest="password", required=True)
    args = parser.parse_args()
    db_specs_path = "/home/popina/Figaro/figaro-code/system_tests/test1/database_specs.conf"
    database = Database(db_specs_path)
    database_name = "DB1"
    database_psql = DatabasePsql(host_name="",user_name="popina", password=args.
                                password, database_name=database_name)
    database_psql.drop_database()
    database_psql.create_database(database)
    database_psql.evaluate_join(database.get_relation_names())

    