import requests
def upload():
    req = requests.post("http://192.168.4.1/upload", files={"file": open("frame.bin", "rb")})
    print(req.status_code, req.reason, req.text)

def clear():
    req = requests.get("http://192.168.4.1/clear")
    print(req.status_code, req.reason, req.text)

if __name__ == "__main__":
    if input() == "upload":
        upload()
        exit()
    else:
        clear()
        exit()