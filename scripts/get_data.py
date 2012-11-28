import pymongo
import os

ROOT_DIR = "."
TRAIN_DIR = ROOT_DIR+"/train"

existing = set(os.listdir(TRAIN_DIR))

mongo = pymongo.Connection('mongodb://sharad:sharad@alex.mongohq.com:10094/handwriting')

db = mongo['handwriting']
characters = db['character']
for doc in characters.find():
    if doc['_id'] not in existing:
        dirname = doc['_id']
        if not os.path.exists(TRAIN_DIR+"/"+dirname):
            os.makedirs(TRAIN_DIR+"/"+dirname)
        for key, value in doc['recordings'].items():
            if key != 'timestamp':
                count = 0
                for example in value:
                    string, delimiter = "", ""
                    filename = key+"-"+str(count)
                    for point in example:
                        string += delimiter + str(point['x']) + " " + str(point['y'])
                        delimiter = "\n"
                    f = open(TRAIN_DIR+"/"+dirname+"/"+filename,'w+')
                    f.write(string)
                    f.close()
                    count += 1
    else:
        print "sup"

