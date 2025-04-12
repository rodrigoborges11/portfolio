
/**
 * A classe Ski representa as perguntas de Ski.
 * Estende a classe Desporto.
 */
class Ski extends Desporto{
    /**
     * Construtor para a classe Ski.
     *
     * @param pergunta A pergunta associada a Ski.
     * @param resposta  Um array de respostas para a pergunta.
     * @param respostaCerta A resposta correta da pergunta.
     * @param pontuacao  A pontuação associada a esta pergunta.
     */
    public Ski(String pergunta, String[] resposta, String respostaCerta , int pontuacao){
        super(pergunta,resposta,respostaCerta,pontuacao);
    }

    /**
     * Método que define a pontuação da pergunta , com a  majoração respetiva (pergunta de Ski: * 2 ) e a majoração extra (pergunta de desporto + 3)
     */
    @Override
    public void majoracao() {
        majoracaoDesporto();
        super.setPontuacao(super.getPontuacao() *2);
    }
}
