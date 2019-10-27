#### Exemplo de adição de textura de elfo, a seguinte linha deve ser colocada no main.cpp: <br>
```objects.push_back( Obj("./elf.obj", "Elf_Albedo.png") );```

<br>

#### (Atenção) Alterei a cor no object.h, mas pode ser que ela tenha que ser alterada, na verdade, no camera.h: <br>
```cor = texture_buffer[y*texture_width + x]```

<br>
#### Obs: Isso é se tiver usando a otimização do array 1D
