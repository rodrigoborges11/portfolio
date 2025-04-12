import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
/**
 * A classe Arte representa as perguntas do tipo Arte
 * Extende a superclasse Perguntas e adiciona funcionalidades especificas (modoEasy , modoHard) para as perguntas do tipo Arte
 */
class Arte extends Perguntas{
    /**
     * Lista de respostas removidas do modoEasy para uso no modoHard.
     */
    private List<String> respostasRemovidas = new ArrayList<>();
    /**
     * Construtor para a classe Arte.
     *
     * @param pergunta      A pergunta relacionada a Arte.
     * @param resposta      Um array de respostas para a pergunta.
     * @param respostaCerta A resposta correta da pergunta.
     * @param pontuacao     A pontuação associada a esta pergunta.
     */
    public Arte(String pergunta,String[] resposta, String respostaCerta, int pontuacao){
        super(pergunta,resposta,respostaCerta,pontuacao);
    }
    /**
     * Método responsável por ativar o modo difícil, adicionando as respostas anteriormente removidas e embaralhando-as.
     */
    public void modoHard(){
        super.getResposta().addAll(respostasRemovidas);
        Collections.shuffle(super.getResposta());
    }
    /**
     * Método responsável por ativar o modo fácil,removendo aleatoriamente respostas até restarem apenas três e embaralhando-as.
     */
    public void modoEasy(){
        while(super.getResposta().size()>3){
            int indice = (int) (Math.random() * super.getResposta().size());
            if(!super.getResposta().get(indice).equals(super.getRespostaCerta())){
                respostasRemovidas.add(super.getResposta().get(indice));
                super.getResposta().remove(indice);
            }
        }
        Collections.shuffle(super.getResposta());
    }
    /**
     * Define a pontuação da pergunta , com a majoração respetiva (pergunta de Arte: * 10 )
     */
    @Override
    public void majoracao() {
        super.setPontuacao(super.getPontuacao() * 10);
    }
}