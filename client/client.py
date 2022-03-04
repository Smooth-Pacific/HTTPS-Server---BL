#!/usr/bin/env python3

import threading
import requests
from requests.auth import HTTPDigestAuth
from datetime import datetime

HOST = "localhost"
PORT = "8080"
USER = "myuser"
PASSWORD = "mypass"
CERT_PATH = "/usr/local/share/ca-certificates/root.crt"
NUM_REQUESTS = 200
NUM_THREADS = 15


def sendRequests():
	s = requests.Session()
	s.auth = HTTPDigestAuth(USER, PASSWORD)
	s.verify = CERT_PATH

	for i in range(NUM_REQUESTS):
		# resp = s.get(f"https://{HOST}:{PORT}/mime/xml")
		# open(f'test{i}.xml', 'wb').write(resp.content)
		# print(resp.status_code)
		resp = s.get(f"https://{HOST}:{PORT}")
		print(resp.status_code, resp.text)


if __name__ == "__main__":
	threads = []
	start = datetime.now()

	for i in range(NUM_THREADS):
		thread = threading.Thread(target=sendRequests)
		thread.start()
		threads.append(thread)

	for t in threads:
		t.join()

	end = datetime.now()
	print(f"{NUM_THREADS * NUM_REQUESTS} requests in {end - start} seconds")

		