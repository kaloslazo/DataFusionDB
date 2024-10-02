# SQL Comandos para testear la base de datos

**OJO** : Es recomendable cerrar y volver a abrir el programa antes de cambiar de índice ya que a veces el frontend puede experimentar problemas de rendimiento.

## Crear una base de datos
```sql
CREATE TABLE songs FROM FILE "./data/spotify_db.csv"
CREATE TABLE cars FROM FILE "./data/cars_db.csv"
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
SELECT * FROM songs WHERE id >= '0001Wtl60puR26ZtSDIF66' AND id <= '00EY0mt7vVQAoPNJiBXsD1'
```

4. Eliminar valores
```
SELECT * FROM songs WHERE id = '000ExtI707Gpbenwj5fffN'
DELETE FROM songs WHERE id = '000ExtI707Gpbenwj5fffN'
```

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
SELECT * FROM songs WHERE id >= '2DTr9iCcuyaowqbC6jjdsr' AND id <= '2HO6j9sszNLEknIYjXO8Fw'
```

4. Eliminar valores
```
SELECT * FROM songs WHERE id = '9191991'
DELETE FROM songs WHERE id = '9191991'
```