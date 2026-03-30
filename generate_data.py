import random
import csv
from datetime import datetime, timedelta

print("Generating 50,000 rows of dummy data...")

with open('data.csv', 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["id", "name", "transaction_amount", "timestamp"])
    
    base_time = datetime.now()
    for i in range(1, 50001):
        name = f"user_{random.randint(1, 1000)}"
        amount = round(random.uniform(10.0, 5000.0), 2)
        timestamp = (base_time + timedelta(seconds=i)).strftime("%Y-%m-%dT%H:%M:%SZ")
        writer.writerow([i, name, amount, timestamp])

print("Done! data.csv is ready.")
