import requests
def req_post():
    req = requests.post("http://192.168.4.1/upload", files={"file": open("frame.bin", "rb")})
    print(req.status_code, req.reason, req.text)