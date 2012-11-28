from pymongo import Connection
from bottle import request, route, run, template, post, static_file
import requests
import urllib2


@post('/upload')
def handle():
    data = urllib2.unquote(request.body.read())
    url = 'https://api.mongohq.com/databases/handwriting/collections/character/documents?_apikey=46hlx1ku4j6kg0ndg8j2'
    headers = {'content-type': 'application/json'}
    print data
    r = requests.post(url, data=data, headers=headers)
    print r.text
    return 'success'

@route('/')
def index():
        return static_file('index.html', root='.')

@route('/<filename>')
def static(filename):
        return static_file(filename, root='.')
run(host='localhost', port=8000)
