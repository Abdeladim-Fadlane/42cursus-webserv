#!/bin/bash

# Send the Content-Type header specifying that the content is HTML
echo "Content-Type: text/html"
# Output an extra blank line to indicate the end of the header
echo -e "\r\n\r\n"  # Use echo -e to enable interpretation of escape sequences
# Output the HTML content
echo "<!DOCTYPE html>"
echo "<html>"
echo "<head>"
echo "<title>Hello CGI Bash</title>"
echo "</head>"
echo "<body>"
echo "<h1>Hello From CGI Bash</h1>"
echo "</body>"
echo "</html>"
