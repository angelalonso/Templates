import csv

def loadCalendar(csv_file):
    ''' Load CSV file into memory
    '''
    calendar_data = []
    with open(csv_file) as csvfile:
        reader = csv.DictReader(csvfile)
        for dct in map(dict, reader):
            calendar_data.append(dct)
    return calendar_data

def printColumn(data, column):
    for entry in data:
        print(entry[column])

printColumn(loadCalendar('data.csv'), 'message')
