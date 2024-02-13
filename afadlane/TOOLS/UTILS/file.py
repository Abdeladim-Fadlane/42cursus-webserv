#!/usr/bin/env python3

# Required header for CGI scripts
print("Content-Type: text/html")  
print()  # Blank line indicates end of headers

# HTML content
print("<html>")
print("<head><title>CGI GET Request</title></head>")
print("<body>")
print("<h1>GET Request Received</h1>")
print("<p>This is a simple CGI script handling a GET request.</p>")

# Retrieving GET parameters
import cgi
form = cgi.FieldStorage()
if "param1" in form:
    param1_value = form.getvalue("param1")
    print(f"<p>Value of 'param1': {param1_value}</p>")
else:
    print("<p>No 'param1' provided in the GET request.</p>")

print("</body>")
print("</html>")
