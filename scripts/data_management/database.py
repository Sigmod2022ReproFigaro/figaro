import json
import logging
import functools

from typing import List
from data_management.relation import Relation

class Database:
    def __init__(self, database_specs_path):
        self.db_config_path = database_specs_path
        with open(database_specs_path) as json_file:
            self.json_db_schema = json.load(json_file)["database"]
        self.load_db_schema()


    def load_db_schema(self):
        self.name = self.json_db_schema["name"]
        json_relations = self.json_db_schema["relations"]
        relations = []
        for json_relation in json_relations:
            relation = Relation(json_relation)
            relations.append(relation)
        self.relations = relations


    def get_relations(self):
        return self.relations


    def sort_relations(self, relation_order: List[Relation]):
        def key_fun(r: Relation):
            return relation_order.index(r.name)
        self.relations.sort(key=key_fun)


    #TODO: Create join tree from query config.
    def set_join_attrs(self):
        for cur_rel in self.relations:
            join_attrs = set()
            for check_rel in self.relations:
                if cur_rel != check_rel:
                    tmp_join_attrs = set(cur_rel.get_attribute_names()).\
                        intersection(set(check_rel.get_attribute_names()))
                    join_attrs = join_attrs.union(tmp_join_attrs)
            cur_rel.set_join_attribute_names(list(join_attrs))

    def get_relation_names(self):
        relation_names = [relation.name for relation in self.relations]
        return relation_names


    def get_relation(self, name):
        for relation in self.relations:
            if relation.name == name:
                return relation
        return None



