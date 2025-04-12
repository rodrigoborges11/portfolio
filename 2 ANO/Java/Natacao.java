/**
 * A classe Natacao representa as perguntas de Natação.
 * Estende a classe Desporto e adiciona funcionalidades específicas (modoEasy , modoHard) para as perguntas do tipo Natação.
 */
class Natacao extends Desporto{
    /**
     * Construtor para a classe Natação
     *
     * @param pergunta A pergunta relacionada à Natação.
     * @param resposta Um array de respostas para a pergunta.
     * @param respostaCerta A resposta correta para a pergunta.
     * @param pontuacao  A pontuação associada a esta pergunta.
     *
     */
    public Natacao(String pergunta, String[] resposta, String respostaCerta , int pontuacao){
        super(pergunta,resposta,respostaCerta,pontuacao);
    }

    /**
     * Define a pontuação da pergunta , com a  majoração respetiva (pergunta de Natacão: + 10 ) e a majoração extra (pergunta de desporto + 3)
     */
    @Override
    public void majoracao() {
        majoracaoDesporto();
        super.setPontuacao(super.getPontuacao() + 10);
    }
}
