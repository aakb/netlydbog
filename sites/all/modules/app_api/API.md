This module adds an API to the site allowing to modify a given users recall list with _get_, _add_ and _delete_ operations. The API uses POST and requires the users _username_, _password_ and library _retailer id_ in every request and all communication is over HTTPS.

The information returned is JSON data with a status and data field. If the status is false an error have occurred and an error code field and a status message is added as well.

This is an error status example:
<pre>
{
  "status" : false,
  "errorcode" : -1,
  "data" : {
    "message" : "User does not exists"
  }
}
</pre>

The API requires an API key to allow access to the methods, which is the _%KEY_ in the URLs below.

## /service/api/%KEY/recall/get

 * Parameters: username=''&password=''&retailer_id=''
 * Type: POST
 * Return: JSON
 * Status: __Implemented__

Get the users recall list in the following format, where the date is the unix timestamp where the item was added to the list.

<pre>
{
  "status" : true,
  "data" : {
    "items" : [
      {
        "isbn" : "9788764461831",
        "title" : "Det glemte brev",
        "date" : "1398162664",
        "description" : "Pella, Siri og Tyra er med klassen på besøg på Helsingborgs Dagblad....",
        "faust" : "29898375",
        "cover" : "http://netlydbog.leela/sites/default/files/covers/29ed382565a322d5b9e361122a293674.jpg"
      },
      {
        "isbn" : "9788711344798",
        "title" : "Sporløs 1 - Dæknavn ... Mathias",
        "date" : "1398162674",
        "description" : "Klokken to om natten bliver Rasmus vækket af sin far....",
        "faust" : "50832724",
        "cover" : "http://netlydbog.leela/sites/default/files/covers/1ec2413af19b05cee8965fb4a0eb42f9.jpg"
      },
      {
        "isbn" : "9788771283846",
        "title" : "De 5 på campingtur",
        "date" : "1398162680",
        "description" : "De 5 består af Julian, Dick, Anne, Georg og hunden Tim....",
        "faust" : "50808238",
        "cover" : "http://netlydbog.leela/sites/default/files/covers/4e795543888fda5a188043f469afb34c.jpg"
      },
      {
        "isbn" : "9788711407349",
        "title" : "Iqbal Farooq og kronjuvelerne",
        "date" : "1398162689",
        "description" : "Arj, hvor er det bare fedt og etniskagtigt at I sådan har en ged med...",
        "faust" : "29037809",
        "cover" : "http://netlydbog.leela/sites/default/files/covers/3e94cd80216b53867c0f49686e365de0.jpg"
      }
    ]
  }
}
</pre>


## /service/api/%KEY/recall/add

 * Parameters: username=''&password=''&retailer_id=''&isbn=''
 * Type: POST
 * Return: JSON
 * Status: __Implemented__
 
Adds a new item to the users recall list based on ISBN. If the operation is successful the following message is returned.

<pre>
{
  "status" : true,
  "data" : {
    "message" : "List have been updated."
  }
}
</pre>

## /service/api/%KEY/recall/del

 * Parameters: username=''&password=''&retailer_id=''&isbn=''
 * Type: POST
 * Return: JSON
 * Status: __Implemented__
 
Removes a item from the users recall list based on ISBN.  If the operation is successful the following message is returned.

<pre>
{
  "status" : true,
  "data" : {
    "message" : "List have been updated."
  }
}
</pre>
