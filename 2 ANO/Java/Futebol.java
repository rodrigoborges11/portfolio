import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
/**
 * A classe Futebol representa as perguntas de Futebol.
 * Estende a classe Desporto e adiciona funcionalidades específicas (modoEasy , modoHard) para as perguntas do tipo Natação.
 */
class Futebol extends Desporto{
    /**
     * Pergunta difícil associada ao desporto de Futebol.
     */
    protected String perguntaDificil;
    /**
     * Lista de respostas para a pergunta difícil.
     */
    protected List<String> respostaDificil = new ArrayList<>();
    /**
     * Resposta correta para a pergunta difícil.
     */
    protected String respostaCertaDificil;

    /**
     * Construtor para a classe Futebol.
     *
     * @param pergunta   A pergunta associada a Futebol.
     * @param perguntaDificil A pergunta difícil associada a Futebol.
     * @param resposta   Um array de respostas para a pergunta.
     * @param respostaDificil  Um array de respostas para a pergunta difícil.
     * @param respostaCerta A resposta correta da pergunta.
     * @param respostaCertaDificil A resposta correta da pergunta difícil.
     * @param pontuacao A pontuação associada a esta pergunta.
     */

    public Futebol(String pergunta,String perguntaDificil,String[] resposta , String[] respostaDificil ,String respostaCerta , String respostaCertaDificil, int pontuacao){
        super(pergunta,resposta,respostaCerta,pontuacao);
        this.perguntaDificil=perguntaDificil;
        this.respostaDificil.addAll(Arrays.asList(respostaDificil));
        this.respostaCertaDificil=respostaCertaDificil;
    }
    /**
     * Método responsável por ativar o modo difícil, alterando a pergunta, respostas e resposta correta para as versões difíceis, embaralhando as respostas.
     */
    public void modoHard(){
        super.setPergunta(perguntaDificil);
        super.setResposta(respostaDificil);
        super.setRespostaCerta(respostaCertaDificil);
        Collections.shuffle(super.getResposta());
    }

    /**
     * Método responsável por ativar o modo fácil, embaralhando as respostas.
     */
    @Override
    public void modoEasy() {
        Collections.shuffle(super.getResposta());
    }

    /**
     * Método que define a pontuação da pergunta , com a  majoração respetiva (pergunta de Futebol: + 1 ) e a majoração extra (pergunta de desporto + 3)
     */
    @Override
    public void majoracao() {
        majoracaoDesporto();
        super.setPontuacao(super.getPontuacao() + 1);
    }
}
