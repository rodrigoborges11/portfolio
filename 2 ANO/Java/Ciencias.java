import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
/**
 * A classe Ciências representa as perguntas do tipo Ciências
 *  Extende a superclasse Perguntas e adiciona funcionalidades especificas (modoEasy , modoHard) para as perguntas do tipo Ciências
 */
class Ciencias extends Perguntas {

    /**
     * Lista de Respostas para o modoHard
     */
    protected List<String> respostaDificil = new ArrayList<>();

    /**
     * Construtor para a classe Ciencias.
     *
     * @param pergunta  A pergunta relacionada a Ciências.
     * @param resposta Um array de respostas para o modoEasy.
     * @param respostaDificil Um array de respostas para o modoHard.
     * @param respostaCerta A resposta correta da pergunta.
     * @param pontuacao  A pontuação associada a esta pergunta.
     */
    public Ciencias (String pergunta,String[] resposta,String[] respostaDificil,String respostaCerta, int pontuacao){
        super(pergunta,resposta,respostaCerta,pontuacao);
        this.respostaDificil.addAll(Arrays.asList(respostaDificil));
    }

    /**
     * Método responsável por ativar o modo difícil, alterando as respostas para a lista de respostas difíceis e embaralhando-as.
     */
    public void modoHard(){
        super.setResposta(respostaDificil);
        Collections.shuffle(super.getResposta());
    }

    /**
     * Método responsável por ativar o modo fácil , embaralhando as perguntas.
     *
     */
    public void modoEasy() {
        Collections.shuffle(super.getResposta());
    }

    /**
     * Define a pontuação da pergunta , com a majoração respetiva (pergunta de Ciências: + 5 )
     */

    public void majoracao(){
        super.setPontuacao(super.getPontuacao() + 5);
    }
}
