import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;


/**
 * A classe topJogadores representa uma lista dos melhores jogadores do jogo POOTRIVIA.
 * Ela armazena objetos do tipo registoJogo, que contêm informações sobre os jogadores.
 */

public class topJogadores implements Serializable {
    /**
     * Lista que armazena os melhores jogadores.
     */
    private List <registoJogo> topJogadores = new ArrayList<>();

    /**
     * Construtor que inicializa a lista de melhores jogadores com um jogador padrão.
     */
    topJogadores(){
        topJogadores.add(new registoJogo());

    }

    /**
     * Obtém a lista dos melhores jogadores.
     *
     * @return Lista dos melhores jogadores.
     */
    public List<registoJogo> getTopJogadores() {
        return topJogadores;

    }
}
