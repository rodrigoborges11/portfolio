import java.util.ArrayList;
import java.util.List;
import java.lang.String;
/**
 * A classe Desporto representa as perguntas do tipo Desporto
 * Extende a superclasse Perguntas
 */
class Desporto extends Perguntas{
    /**
     * Construtor para a classe Desporto.
     *
     * @param pergunta A pergunta relacionada a Desporto.
     * @param resposta  Um array de respostas para a pergunta.
     * @param respostaCerta A resposta correta da pergunta.
     * @param pontuacao  A pontuação associada a esta pergunta.
     */
    public Desporto(String pergunta, String[] resposta, String respostaCerta , int pontuacao){
        super(pergunta,resposta,respostaCerta,pontuacao);
    }
    /**
     * Define a pontuação da pergunta , com a majoração respetiva (pergunta de Desporto: + 2 )
     */
    public void majoracaoDesporto() {
        super.setPontuacao(super.getPontuacao() + 3);
    }

}




