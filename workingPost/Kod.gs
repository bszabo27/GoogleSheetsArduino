//Fields to be customized for yourself
var spreadsheetID = SpreadsheetApp.openById('<sheetID>');
var sheetlocalizedname = spreadsheetID.getSheetByName('Munkalap1');
var str = "";

//A code qualifies as a scipt if it has either doGet() or doPost() methods and returns a ContentService.
// In this example we have both.

function doPost(e) {

  var parsedData;
  var result = {};
  
  try { 
    parsedData = JSON.parse(e.postData.contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }
   
  if (parsedData !== undefined){
    // Common items first
    // data format: 0 = display value(literal), 1 = object value
    var flag = parsedData.format;
    
    if (flag === undefined){
      flag = 0;
    }
    
    //Get the CSV, and add as a row.
    switch (parsedData.command) {
      case "appendRow":
         var tmp = spreadsheetID.getSheetByName(parsedData.sheet_name);
         var nextFreeRow = tmp.getLastRow() + 1;
         var dataArr = parsedData.values.split(",");
         
         tmp.appendRow(dataArr);
         
         str = "Success";
         SpreadsheetApp.flush();
         break;               
    }
    
    return ContentService.createTextOutput(str);
  } 
  
  else{
    return ContentService.createTextOutput("Error. Issue with the request body!");
  }  
  
}

function doGet(e){
  //Get requests cannot have a HTTP Body, so params need to be passed via URI queryparams  
  var val = e.parameter.value;
  var read = e.parameter.read; 
  
  if (read !== undefined){
    //It is possible but not recommended to write anything with a GET request besides logs.
    var now = Utilities.formatDate(new Date(), "EST", "yyyy-MM-dd'T'hh:mm a'Z'").slice(11,19);
    sheetlocalizedname.getRange('D1').setValue(now);
    var count = (sheetlocalizedname.getRange('C1').getValue()) + 1;
    sheetlocalizedname.getRange('C1').setValue(count);
    //Actually GETting values is okay.
    return ContentService.createTextOutput(sheetlocalizedname.getRange('A1').getValue());
  }
  
  if (e.parameter.value === undefined)   
    return ContentService.createTextOutput("No value passed as argument.");
    
  //It is possible but not recommended to write anything with a GET request besides logs
  //Especially do not write userinputs from queryParams for the IT Gods' sake
  var range = sheetlocalizedname.getRange('A1');
  var retval = range.setValue(val).getValue();
  var now = Utilities.formatDate(new Date(), "EST", "yyyy-MM-dd'T'hh:mm a'Z'").slice(11,19);
  sheetlocalizedname.getRange('B1').setValue(now);
  sheetlocalizedname.getRange('C1').setValue('0');
  
  if (retval == e.parameter.value)
    return ContentService.createTextOutput("Successfully wrote: " + e.parameter.value );
  else
    return ContentService.createTextOutput("Error while writing into spreadsheet.\nPlease check auth." + retval + ' ' + e.parameter.value);
}

function onOpen(){
  var ui = SpreadsheetApp.getUi();
  ui.createMenu('ESP8266 Logging')
  .addItem('Clear', 'Clear')
  .addToUi();
}

function Clear(){
  sheetlocalizedname.deleteRows(4, sheetlocalizedname.getLastRow());
  spreadsheetID.toast('Chart cleared', 'ESP8266 logging', 5);
}


