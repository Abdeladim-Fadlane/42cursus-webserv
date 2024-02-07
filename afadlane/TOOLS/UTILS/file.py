import requests

def send_get_request(url, params=None):
    try:
        response = requests.get(url, params=params)
        response.raise_for_status()  # Raise an exception for bad status codes
        return response.text
    except requests.exceptions.RequestException as e:
        print("Error:", e)
        return None

# Example usage
cgi_url = "http://google.com"
parameters = {
    'param1': 'value1',
    'param2': 'value2'
}

response_text = send_get_request(cgi_url, params=parameters)
if response_text:
    print("Response from CGI script:")
    print(response_text)
else:
    print("Failed to get response from CGI script.")
