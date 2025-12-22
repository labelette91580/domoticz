import csv
import time
import requests

def replay_csv_to_domoticz(
        csv_file,
        domoticz_url,
        idx_temp_int,
        idx_temp_ext,
        separator=';',
        delay_sec=60
    ):
    """
    Lit un CSV et met à jour Domoticz chaque minute
    colonne 2 = température intérieure
    colonne 3 = température extérieure
    """

    with open(csv_file, newline='', encoding='utf-8') as f:
        reader = csv.reader(f, delimiter=separator)

        header = next(reader, None)  # ignorer l'en-tête

        for row in reader:
            try:
                temp_int = float(row[1])
                temp_ext = float(row[2])
            except (IndexError, ValueError):
                print("Ligne invalide ignorée :", row)
                continue

            # Température intérieure
            url_int = (
                f"{domoticz_url}/json.htm?"
                f"type=command&param=udevice"
                f"&idx={idx_temp_int}"
                f"&nvalue=0"
                f"&svalue={temp_int};0;0"
            )

            # Température extérieure
            url_ext = (
                f"{domoticz_url}/json.htm?"
                f"type=command&param=udevice"
                f"&idx={idx_temp_ext}"
                f"&nvalue=0"
                f"&svalue={temp_ext}"
            )

            try:
                requests.get(url_int, timeout=5)
                requests.get(url_ext, timeout=5)

                print(f"MAJ Domoticz → Int={temp_int}°C / Ext={temp_ext}°C")

            except requests.RequestException as e:
                print("Erreur Domoticz :", e)

            time.sleep(delay_sec)


csv_file="temperature_log.csv"
domoticz_url="http://127.0.0.1:8080"
idx_temp_int="Salle2"
idx_temp_ext="Exterieur"

replay_csv_to_domoticz(
        csv_file,
        domoticz_url,
        idx_temp_int,
        idx_temp_ext,
        separator=';',
        delay_sec=60)