import link_connection;

def insert_song():
    nombre = input("Insert song name: ");
    song_id = int(input("Insert song id: "));

    db_result = link_connection.insert_record(nombre, song_id);
    
    if (db_result == 0):
        print("Song inserted successfully");
    else:
        print("Error inserting song");

if __name__ == "__main__":
    insert_song();
