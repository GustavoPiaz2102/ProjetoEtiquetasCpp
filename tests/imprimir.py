
def gerar_etiqueta():
    return """SIZE 60 mm,40 mm
GAP 2 mm,0
DENSITY 8
SPEED 4
DIRECTION 1
CLS
TEXT 100,100,"3",0,1,1,"L.:001/25"
TEXT 100,150,"3",0,1,1,"FAB.:23/JAN/2025"
TEXT 100,200,"3",0,1,1,"VAL.:23/JAN/2027"
PRINT 1
"""

def imprimir_etiqueta(comando):
    try:
        with open("/dev/usb/lp0", "wb") as impressora:
            impressora.write(comando.encode())  # UTF-8 normalmente é aceito
        print("✅ Etiqueta enviada com sucesso!")
    except Exception as e:
        print(f"❌ Erro ao imprimir: {e}")

if __name__ == "__main__":
    comando = gerar_etiqueta()
    imprimir_etiqueta(comando)
