import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.lang.String;
/**
 * A classe Perguntas é a superclasse que representa uma pergunta genérica do jogo.
 * Contém a pergunta, as opções de resposta, a resposta correta e a pontuação associada.
 */
public class Perguntas {
    /**
     *Pergunta
     */
    private String pergunta;
    /**
     * Pontuação
     */
    private int pontuacao;
    /**
     * Opções de Resposta
     */

    private List<String> resposta = new ArrayList<>();
    /**
     * Opção certa
     */
    private String respostaCerta;

    /**
     * Construtor para a classe Perguntas.
     *
     * @param pergunta  pergunta
     * @param resposta  Um array de opções de resposta
     * @param respostaCerta resposta correta
     * @param pontuacao     pontuação
     */
    public Perguntas(String pergunta, String[] resposta, String respostaCerta, int pontuacao) {
        this.pergunta=pergunta;
        this.resposta.addAll(Arrays.asList(resposta));
        this.respostaCerta=respostaCerta;
        this.pontuacao=pontuacao;
    }

    public void setPontuacao(int pontuacao) {
        this.pontuacao = pontuacao;
    }

    public void setPergunta(String pergunta) {
        this.pergunta = pergunta;
    }

    public void setResposta(List<String> resposta) {
        this.resposta = resposta;
    }

    public void setRespostaCerta(String respostaCerta) {
        this.respostaCerta = respostaCerta;
    }

    public int getPontuacao() {
        return pontuacao;
    }

    public String getPergunta() {
        return pergunta;
    }

    public List<String> getResposta() {
        return resposta;
    }

    public String getRespostaCerta() {
        return respostaCerta;
    }
    /**
     * Método para ativar o modoHard.
     */
    public void modoHard(){};
    /**
     * Método para ativar o modoEasy.
     */
    public void modoEasy() {
    }
    /**
     * Método para definir uma determinada majoração na pontuação.
     */
    public void majoracao(){}
}