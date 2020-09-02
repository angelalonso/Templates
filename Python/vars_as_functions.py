import sys
import yaml

verbs_in_use = ['get', 'add', 'update', 'delete']


def loadObjectsStruct(objects_file):
    try:
        with open(objects_file) as file:
            objects = yaml.load(file, Loader=yaml.FullLoader)
    except FileNotFoundError:
        return None
    return objects


def loadArgs(verbs, objects_struct):
    objects = []
    for key in objects_struct:
        objects.append(key)
    try:
        # First arg should be the Verb
        if sys.argv[1] in verbs:
            verb = sys.argv[1]
            try:
                # Second arg should be the Object
                if sys.argv[2] in objects:
                    obj = sys.argv[2]
                    params = sys.argv[3:]
                    return verb, obj, params
                else:
                    showError("OBJECT " + sys.argv[2] + " could not be understood.", verbs, objects)
            except IndexError:
                showError("OBJECT is missing.", verbs, objects)
        else:
            showError("VERB " + sys.argv[1] + " could not be understood.", verbs, objects)
    except IndexError:
        showError("VERB is missing.", verbs, objects)

def get():
    print("GET")


def add():
    print("ADD")


def showError(message, verbs, objects):
    print("ERROR!")
    print(message + "\n")
    showHelp(verbs, objects)
    sys.exit(2)


def showHelp(verbs, objects):
    print("Usage: " + sys.argv[0] + " [VERB] [OBJECT] [parameters]")
    print("  Available VERB(s):")
    for verb in verbs:
        print("  - " + verb)
    print("  Available OBJECT(s):")
    for obj in objects:
        print("  - " + obj)


if __name__ == '__main__':
    objects_file = 'objects.yaml'
    objects_struct = loadObjectsStruct(objects_file)
    verb, obj, params = loadArgs(verbs_in_use, objects_struct)
    globals()[verb]()

