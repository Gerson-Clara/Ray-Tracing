print("\n==========================================")
print("    BEM-VINDO AO RAY TRACER C/LUA    ")
print("==========================================\n")

print("[LUA] Esse script esta rodando sobre o motor C.")
print("[LUA] Sera gerada a imagem de uma esfera com sombreamento.")

-- Parametros da imagem (pode alterar aqui se quiser)
local largura = 300
local altura = 300
local nome_arquivo = "resultado_esfera.ppm"

print("\n[LUA] Solicitadno ao motor C pra iniciar os calculos...")
print(string.format("[LUA] Parametros enviados: %dx%d -> %s", largura, altura, nome_arquivo))

-- A funcao meuraytracer.gerar_imagem nao existe no Lua,
-- ela eh a ponte pro codigo C
local sucesso, erro_msg = meuraytracer.gerar_imagem(largura, altura, nome_arquivo)

print("\n[LUA] O motor C retornou o controle para o script Lua.")

if sucesso then
    print("\n==========================================")
    print(" SUCCESSO! Imagem gerada.")
    print(" Arquivo salvo como " .. nome_arquivo)
    print(" OBS: Para visualizar o arquivo .ppm use uma extensao ou visualizador que suporte esse formato.")
    print("==========================================\n")
else
    print("\n[LUA] ERRO: O motor C falhou por algum motivo.")
    print("Detalhes: " .. (erro_msg or "Erro desconhecido"))
end