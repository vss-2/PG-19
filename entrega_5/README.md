#### Exemplo de adição de textura de elfo, a seguinte linha deve ser colocada no main.cpp: <br>
```objects.push_back( Obj("./elf.obj", "Elf_Albedo.png") );```

```----------------------------------------------------------------------------------------------```

#### (Atenção) Alterei a cor no object.h, mas pode ser que ela tenha que ser alterada, na verdade, no camera.h: <br>
```cor = texture_buffer[y*texture_width + x]```

#### Obs: Isso é se tiver usando a otimização do array 1D

```----------------------------------------------------------------------------------------------```

#### Dentro do for do for que a gente vai fazendo o Phong, é preciso botar:
```vt = wa*V0.vt + wb*V1.vt + wb*V2.vt```

#### Isso serve para pegar a normal de cada ponto para colocar a textura.
#### Obs: coordenada da textura != coordenada da tela, a coordenada y da textura é 1-y (ela cresce a partir do canto inferior.

```----------------------------------------------------------------------------------------------```

#### O ponto x e y das coordenadas de vt devem ser multiplicados pela width e height da imagem:
```
vt.x() = vt.x() * imgWidth;
vt.y() = vt.y() * imgHeight;
```

#### Obs: Os .x() e .y() em vt são implementados como float, já ajustei para int no texture_buffer do object.h, como avisado no slide.

