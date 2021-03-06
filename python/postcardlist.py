'''
Exam requests: 
1. Write a class PostcardList that satisfies the tests (defined using unittest). 
SPECIFICATIONS: 
Class PostcardList reads/writes Postcard messages from/to a properly formatted file: each record (i.e., file's line) is a Postcard. 
The Postcard format is "date:$(DATE); from:$(SENDER); to:$(RECEIVER);" (e.g., "date:2009-12-24; from:Daisy; to:Donald;"). For the sake of simplicity, the message is not considered.
Class PostcardList must have at least these attributes:
 - _file: file name, eventually with the full path.
 - _postcards: list of postcards read from _file.
 - _date: is a dict where the key is the string date, and the value is a list of indices. Each index refers to the corresponding record.
 - _from: is a dict where the key is the string sender, and the value is a list of indices. Each index refers to the corresponding record.
 - _to:   is a dict where the key is the string receiver, and the value is a list of indices. Each index refers to the corresponding record.
Class PostcardList must manage the I/O file through the following member functions. Note that you are free to choose the appropriate arguments, but the function names must be as follows:  
 - writeFile(self,...): write self.{_date,_from,_to} to self._file  
 - readFile(self,...): from self._file read self.{_date,_from,_to}    
 - parsePostcards(self,...): parse self._postcards, set self.{_date,_from,_to} 
 - updateFile(self,...): as write but appending to self._file 
 - updateLists(self,...): as read but appending to self._postcards 
 - getNumberOfPostcards(self): returns length of self._postcards
Class PostcardList must manage the sorting of dates/senders/receivers. Note that the names and arguments of the functions are fixed as follows: 
 - getPostcardsByDateRange(self,date_range): returns the postcards within a date_range
 - getPostcardsBySender(self, sender): returns the postcards from a sender
 - getPostcardsByReceiver(self, receiver): returns the postcards to a receiver
'''
import unittest  #unit test
import datetime  #date type
import re        #regex
class PostcardList:
  
  def __init__(self):
    
    self._index   : int  = 0
    self._file    : str  = ""       #file name, eventually with the full path.
    self._postcards : list = []  #list of postcards (ordereddicts) read from _file.
    self._date    : dict = {}       #a dict where the key is the string date, and the value is a list of indices. Each index refers to the corresponding record.
    self._from    : dict = {}       #is a dict where the key is the string sender, and the value is a list of indices. Each index refers to the corresponding record.
    self._to    : dict = {}         #a dict where the key is the string receiver, and the value is a list of indices. Each index refers to the corresponding record.
    
  
########helper functions#########

  def parsePostcards(self, line: str) -> dict:
    '''
    input: string "date:$(DATE); from:$(SENDER); to:$(RECEIVER);""
    output: dict  {"index" : int, 
                   "sender"    : str, 
                   "receiver"  : str, 
                   "date"      : datetime.date}
    '''
    
    #gets the first 3 numbers of the date, and whatever is after from: and to:, including $ and .'s
    pattern = re.compile("date:(\d*)-(\d*)-(\d*).*from:([^;]+); to:([^;]+);")
    #regex extraction of useful data:
    regexItems = re.search(pattern,line)
    #1,2,3 -> Y,m,d , 4-> sender , 5-> receiver

    #assembling date string, aliasing sender and receiver for readability
    _date = regexItems.group(1) + " " + regexItems.group(2) + " " + regexItems.group(3)
    _sender = regexItems.group(4)
    _receiver = regexItems.group(5)
    
    return {"index" : self._index, "sender": _sender, "receiver": _receiver, "date" : datetime.datetime.strptime(_date, "%Y %m %d")}

  def formatted(self, message: dict) -> str:
    '''
    input:  dict message
    output: test-compliant string message
    '''
    return "date:{}; from:{}; to:{};\n".format(datetime.datetime.strftime(message["date"],"%Y-%m-%d"), message["sender"], message["receiver"])
   
    
 ##################end helper functions#########

  
  
  def writeFile(self, outputFile):
    '''
    input: none
    output: write self.{_date,_from,_to} to self._file
    
    the specifics delete the list of messages from self._file, and overwrites them with the three dicts.
    to avoid input file destruction, the possibility to write to a different output file has been implemented instead.
    to remain compliant with the specifications, the original file name must be provided, as a safety measure
    
    '''
    
    with open(outputFile, 'w') as file:  # overwrites file
      for message in self._postcards:
        #formats the line and write it in the file:
        line = self.formatted(message)
        file.write(line)
  
  def readFile(self, *args): 
    '''
    input:  string file name with path
    output: from self._file read self.{_date,_from,_to}'''
    
    #check if a str filename is provided. In that case, read file. Otherwise, read the already attached file.
    if len(args) == 0:
        pass
    elif len(args) == 1 and isinstance(args[0], str):
        self._file = args[0]
    elif len(args) > 1:
      raise IOError
    
    #initialize attributes
    self._index   : int  = 0
    self._postcards : list = []
    self._from    : dict = {}
    self._to    : dict = {}
    self._date    : dict = {}
    
    with open(self._file, 'r') as file:
      for line in list(file):
        message = self.parsePostcards(line)
        self._postcards.append(message)
        self._index +=1
        
  #adds message to "from" dict
        if (message["sender"] in self._from.keys()):
          self._from[message["sender"]].append(message["index"])
        else:
          self._from[message["sender"]]=[message["index"]]

  #adds message to "to" dict
        if (message["receiver"] in self._to.keys()):
          self._to[message["receiver"]].append(message["index"])
        else:
          self._to[message["receiver"]]=[message["index"]]

  #adds message to "date" dict
        if (message["date"] in self._date.keys()):
          self._date[message["date"]].append(message["index"])
        else:
          self._date[message["date"]]=[message["index"]]
   
  def updateFile(self, outputFile): 
    '''
    appends current postcards to attached file
    '''
    with open(outputFile, 'a') as file: # appends to file
      
      for message in self._postcards:
        #"date:$(DATE); from:$(SENDER); to:$(RECEIVER);"
        line = self.formatted(message)
        file.write(line)
  
  def updateLists(self, *args): 
    '''as read but appending to self._postcards'''
    
#check if a str filename is provided. In that case, read file.
    if len(args) == 0:
        pass
    elif len(args) == 1 and isinstance(args[0], str):
        self._file = args[0]
    elif len(args) > 1:
      raise IOError
    
# self._postcards=[]   #commented to allow append
    with open(self._file, 'r') as file:
      for line in list(file):
        self._postcards.append(self.parsePostcards(line))    
  def getNumberOfPostcards(self): 
    '''returns length of self._postcards'''
    return len(self._postcards)
  
  def getPostcardsByDateRange(self,date_range : datetime.timedelta) -> list: 
    '''
    returns the postcards within a date_range, date_rage is a tuple of 2 date types
    ''' 
    begin, end = date_range
    return [self.formatted(m) for m in self._postcards if (m["date"] > begin and m["date"] < end)]
  
  def getPostcardsBySender(self, sender: str) -> list: 
    '''
    returns the messages from a sender
    '''    
    return [self.formatted(m) for m in self._postcards if (m["sender"] == sender)]
    
  
  def getPostcardsByReceiver(self, receiver: str) -> list: 
    '''
    returns the postcards to a receiver 
    '''    
    return [self.formatted(m) for m in self._postcards if (m["receiver"] == receiver)]

########################
# TO COMMENT 
# from exam_solution import PostcardList # overrides the class define above
########################


class Test(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self._files = ['./exam_postcard_list{}.txt'.format(i) \
                           for i in range(10)]
        self._PstLst = [PostcardList() for f in self._files]
        [p.readFile(f) for f,p in zip(self._files,self._PstLst)]

    def test_missing_attributes(self): 
        attributes_to_define = ['_file','_postcards','_date','_from','_to',\
                                'writeFile','readFile','parsePostcards',\
                                'updateFile','updateLists',\
                                'getNumberOfPostcards',\
                                'getPostcardsByDateRange',\
                                'getPostcardsBySender',\
                                'getPostcardsByReceiver']
        for attr in attributes_to_define:
            if attr not in dir(self._PstLst[0]): 
                raise Exception(attr+' is missing')

    def test_check_getPostcardByDateRange(self): 
        dr_test = []
        for i in [0,1,4,6]: 
            dr_test.append(\
                self._PstLst[i].getPostcardsByDateRange(date_range=\
                   (datetime.datetime.strptime('2008-1-1', "%Y-%m-%d"),\
                    datetime.datetime.strptime('2010-12-31', "%Y-%m-%d"))))
        self.assertListEqual(sorted(dr_test[0]),sorted(['date:2010-06-23; from:Sneezy; to:Alice;\n', 'date:2009-12-12; from:Dopey; to:Peter;\n', 'date:2008-03-23; from:Sneezy; to:Pluto;\n', 'date:2008-06-03; from:Goofy; to:Pluto;\n'])) 
        self.assertEqual(len(dr_test[1]),1)
        self.assertListEqual(sorted(dr_test[2]),sorted(['date:2010-03-30; from:Louie; to:Sneezy;\n', 'date:2010-03-05; from:Goofy; to:Dopey;\n', 'date:2009-11-08; from:Daisy; to:Minnie;\n', 'date:2010-07-13; from:Bashful; to:Louie;\n', 'date:2008-06-29; from:Huey; to:Dopey;\n', 'date:2009-01-04; from:Alice; to:Hook;\n', 'date:2009-04-28; from:Bashful; to:Mickey;\n']))
        self.assertEqual(len(dr_test[3]),7)

    def test_check_getPostcardBySender(self): 
        s_test = []
        for i in [2,3,5,7]:
            for sender in ['Daisy','Bashful','Peter']:
                s_test.append(\
                    self._PstLst[i].getPostcardsBySender(sender=sender))
        self.assertIn('date:2011-12-11; from:Daisy; to:Goofy;\n',s_test[0])
        self.assertListEqual(s_test[1],[])
        self.assertEqual(len(s_test[2]),1)
        self.assertEqual(len(s_test[5]),0)
        self.assertGreater(len(s_test[8]),len(s_test[9]))

    def test_check_getPostcardByReceiver(self): 
        r_test = []
        for i in [0,3,8,9]:
            for receiver in ['Peter','Hook','Alice','SnowWhite']:
                r_test.append(\
                  self._PstLst[i].getPostcardsByReceiver(receiver=receiver))
        self.assertIn('date:2009-12-12; from:Dopey; to:Peter;\n',r_test[0])
        self.assertListEqual(r_test[1],['date:2016-10-23; from:Sneezy; to:Hook;\n'])
        self.assertEqual(len(r_test[2]),2)
        self.assertEqual(len(r_test[6]),3)

    def test_check_sent_received_when(self): 
        srw_test = []
        for ii in range(10):
            for jj in ["Mickey","Minnie","Goofy","Pluto","Donald","Daisy","$crooge","Huey","Dewey","Louie","Peter","Hook","Alice","SnowWhite","Doc","Sleepy","Sneezy","Bashful","Dopey","Happy","Grumpy"]:
                try:
                    srw_test.append(self._PstLst[ii]._from[jj])
                    srw_test.append(self._PstLst[ii]._to[jj])
                    srw_test.append(self._PstLst[ii]._date[jj])
                except: pass
        self.assertListEqual(srw_test[0],[1])
        self.assertListEqual(srw_test[130],[14,25])
        self.assertListEqual(srw_test[138],[10])
        self.assertListEqual(srw_test[140],[15])
        self.assertListEqual(srw_test[192],[13,26])
        self.assertListEqual(srw_test[203],[6, 9, 11, 12, 24, 31, 42])

if __name__ == '__main__':

  unittest.main()