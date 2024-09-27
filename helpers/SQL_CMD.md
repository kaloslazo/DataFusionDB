# SQL Comandos para testear la base de datos

## Crear una base de datos
```sql
CREATE TABLE songs FROM FILE "./data/spotify_data_clean.csv"
CREATE TABLE songs FROM FILE "./data/car_prices_demo.csv"
```

---

## Indice extendible hashing
1. Crear índice
```
CREATE INDEX idx_songs ON songs USING HASH
```

2. Insertar valores
```
INSERT INTO songs VALUES ('101010', 'Despacito', 'Vida', 'ALB001', 'Luis Fonsi')
SELECT * FROM songs WHERE id = '101010'
```

3. Consultar valores
```
SELECT * FROM songs WHERE id = '4VBryMYpnuNBvfhDuS3c4L'
SELECT * FROM songs WHERE id = '5Wdc8WS8jgEwgVw5dXTohG'
```

4. Eliminar valores
```
DELETE FROM songs WHERE id = '101010'
SELECT * FROM songs WHERE id = '101010'
```

5. Operaciones no soportadas
```
SELECT * FROM songs
SELECT * FROM songs WHERE id >= '1' AND id <= '696969'
SELECT * FROM songs WHERE name = 'Shape of You'
```

---

## Indice AVL
1. Crear índice
```
CREATE INDEX idx_songs ON songs USING AVL
```

2. Insertar valores
```
INSERT INTO songs VALUES ('9191991', 'Watermelon Sugar', 'Fine Line', 'ALB004', 'Harry Styles')
SELECT * FROM songs WHERE id = '9191991'
```

3. Consultar valores
```
SELECT * FROM songs
SELECT * FROM songs WHERE id = '3'
SELECT * FROM songs WHERE id >= '1' AND id <= '4'
SELECT * FROM songs WHERE name = 'Watermelon Sugar'
```

4. Eliminar valores
```
DELETE FROM songs WHERE id = '9191991'
SELECT * FROM songs
```

---

## Indice secuencial

1. Crear índice
```
CREATE INDEX idx_songs ON songs USING SEQUENTIAL
```

2. Insertar valores
```
INSERT INTO songs VALUES ('7443958', 'Circles', 'Hollywood's Bleeding', 'ALB007', 'Post Malone')
SELECT * FROM songs WHERE id = '7443958'
```

3. Consultar valores
```
SELECT * FROM songs
SELECT * FROM songs WHERE id = '7443958'
SELECT * FROM songs WHERE id >= '3' AND id <= '6'
SELECT * FROM songs WHERE album = 'After Hours'
```

4. Eliminar valores
```
DELETE FROM songs WHERE id = '4'
SELECT * FROM songs
```
