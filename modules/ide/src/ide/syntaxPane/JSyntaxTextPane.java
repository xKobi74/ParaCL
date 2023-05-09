/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ide.syntaxPane;

/**
 *
 * @author ilya
 */
import java.awt.Color;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.JFrame;
import javax.swing.JTextPane;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.TabSet;
import javax.swing.text.TabStop;

/**
 * This class is a prototype for a syntax highlighter for java code.
 * It highlights common java keywords, boolean values and it highlights digits in the text.
 * 
 * Limitations of the current prototype:
 * -It does not highlight comments
 * -It does not highlight method calls
 * -It does not highlight objects that are not a part of common java keywords
 * -It does not have intellisense autosuggestion
 * 
 * Addendum:
 * Even though this syntax highlighter is designed for java code, {@link #initializeSyntaxHighlighter()} can be modified to highlight any other programming language or keywords
 */
public class JSyntaxTextPane extends JTextPane {

    // Default Styles
    final StyleContext cont = StyleContext.getDefaultStyleContext();
    AttributeSet defaultForeground = cont.addAttribute(cont.getEmptySet(), StyleConstants.Foreground, Color.black);
    AttributeSet defaultNumbers = cont.addAttribute(cont.getEmptySet(), StyleConstants.Foreground, new Color(115, 214, 34));
    AttributeSet defaultComment = cont.addAttribute(cont.getEmptySet(), StyleConstants.Foreground, Color.gray);
    private boolean isStyled = true;
    
    
    public void setStyled(boolean styled) {
        isStyled = styled;
    }
    public JSyntaxTextPane () {
        // Styler
        DefaultStyledDocument doc = new DefaultStyledDocument() {
            
            private void highlightComments() throws BadLocationException{
            String text = addTerminate(getText(0, getLength()));

            Pattern pattern = Pattern.compile("//.*");
            Matcher matcher = pattern.matcher(text);
            while (matcher.find()) {
                //System.out.println(text.substring(matcher.start(), matcher.end()));
                int end = text.indexOf('\n', matcher.start());
                System.out.println(text.substring(matcher.start(), end));
                setCharacterAttributes(matcher.start(), matcher.end() - matcher.start(), defaultComment, false);
            }
            
            }
            
            private void highlightInsert(int offset, String str, AttributeSet a) throws BadLocationException {
                String text = addTerminate(getText(0, getLength()));
                int before = findLastNonWordChar(text, offset);
                if (before < 0) before = 0;
                int after = findFirstNonWordChar(text, offset + str.length());
                int wordL = before;
                int wordR = before;

                while (wordR <= after) {
                    if (wordR == after || String.valueOf(text.charAt(wordR)).matches("\\W")) {
                        // Colors words in appropriate style, if nothing matches, make it default black
                        boolean matchFound = false;
                        
                        for (KeyWord keyWord : keyWords) {
                            if (text.substring(wordL, wordR).matches("(\\W)*("+keyWord.getWords()+")")) {
                                setCharacterAttributes(wordL, wordR - wordL, keyWord.getColorAttribute(), false);
                                matchFound = true;
                            }
                        }
                        
                        // Highlight numbers
                        if (text.substring(wordL, wordR).matches("\\W\\d[\\d]*")) {
                            setCharacterAttributes(wordL, wordR - wordL, defaultNumbers, false);
                            matchFound = true;
                        }
                       
                        // If no match found, make text default colored
                        if(!matchFound) {
                            setCharacterAttributes(wordL, wordR - wordL, defaultForeground, false);
                        }
                        
                        wordL = wordR;
                    }
                    
                    wordR++;
                }
            }
            @Override
            public void insertString (int offset, String str, AttributeSet a) throws BadLocationException {
                super.insertString(offset, getDeveloperShortcuts(str), a);
                if (isStyled) {
                    highlightInsert(offset, str, a);
                    highlightComments();
                } else {
                    setCharacterAttributes(0, getLength(), defaultForeground, false);
                }
            }
            
            private void highlightRemove(int offs, int len) throws BadLocationException {
                String text = addTerminate(getText(0, getLength()));
                int before = findLastNonWordChar(text, offs);
                if (before < 0) before = 0;
                int after = findFirstNonWordChar(text, offs);

                // Colors words in appropriate style, if nothing matches, make it default black
                boolean matchFound = false;
                for (KeyWord keyWord : keyWords) {
                    if (text.substring(before, after).matches("(\\W)*("+keyWord.getWords()+")")) {
                        setCharacterAttributes(before, after - before, keyWord.getColorAttribute(), false);
                        matchFound = true;
                    } 
                    
                    // Highlight numbers
                    if (text.substring(before, after).matches("\\W\\d[\\d]*")) {
                        setCharacterAttributes(before, after - before, defaultNumbers, false);
                        matchFound = true;
                    }
                    
                    if(!matchFound) {
                        setCharacterAttributes(before, after - before, defaultForeground, false);
                    }
                }
            }
            @Override
            public void remove (int offs, int len) throws BadLocationException {
                super.remove(offs, len);
                if (isStyled) {
                    highlightRemove(offs, len);
                    highlightComments();
                } else {
                    setCharacterAttributes(0, getLength(), defaultForeground, false);
                }
                
            }
        };
        
       
        
        
        
        
        setStyledDocument(doc);
        
        // SET DEFAULT TAB SIZE
        setTabSize(40);

        // THIS PART APPLIES DEFAULT SYNTAX HIGHLIGHTER BEHAVIOUR
        initializeSyntaxHighlighter();
    }
    
    private String addTerminate(String text) {
        if (!text.endsWith("\n")) {
            text += "\n";
        }
        return text;
    }
    
    private int findLastNonWordChar (String text, int index) {
        while (--index >= 0) {
            if (String.valueOf(text.charAt(index)).matches("\\W")) {
                break;
            }
        }
        return index;
    }

    private int findFirstNonWordChar (String text, int index) {
        while (index < text.length()) {
            if (String.valueOf(text.charAt(index)).matches("\\W")) {
                break;
            }
            index++;
        }
        return index;
    }
    
    /**
     * Shortcuts, when letter is typed, it will produce additional strings specified inside of this method
     * 
     * @param str
     * @return
     */
    private String getDeveloperShortcuts(String str) {
        // Add ending parenthesis when it is open
        if(str.equals("(")) {
            return "( )";
        }
        
        // Add ending braces when it is open
        if(str.equals("{")) {
            return "{ }";
        }
        
        return str;
    }
    
    /**
     * Sets size of space produced when user presses Tab button
     * 
     * @param tabSize
     */
    public void setTabSize(int tabSize) {
        // Once tab count exceed x times, it will make a small space only
        int maxTabsPerRow = 10;
        
        TabStop[] tabs = new TabStop[maxTabsPerRow];
        for(int i = 0; i < maxTabsPerRow; i++) {
            tabs[i] = new TabStop(tabSize*(i+1), TabStop.ALIGN_LEFT, TabStop.LEAD_NONE);
        }
        
        TabSet tabset = new TabSet(tabs);
        
        StyleContext sc = StyleContext.getDefaultStyleContext();
        AttributeSet aset = sc.addAttribute(SimpleAttributeSet.EMPTY,
        StyleConstants.TabSet, tabset);
        setParagraphAttributes(aset, false);
    }
    
    /**
     * Adds a key word or keywords that will be colored in the JTextPane
     * 
     * @param color - color of the words
     * @param words - words that need to be colored
     */
    public void addKeyWord(Color color, String ...words) {
        keyWords.add(new KeyWord(color, words));
    }
    
    ArrayList<KeyWord> keyWords = new ArrayList<KeyWord>();
    
    /**
     * Holds information about list of words that need to be colored in a specific color
     * 
     */
    class KeyWord {
        Color color;
        String[] words;
        
        /**
         * Instantiates a new key word object that holds a list of words that need to be colored.
         *
         * @param color the color
         * @param words the words
         */
        public KeyWord(Color color, String...words) {
            this.color = color;
            this.words = words;
        }
        
        public String getWords() {
            String text = "";
            
            for (int i = 0; i < words.length; i++) {
                if(i != words.length-1) {
                    text = text + words[i] + "|";
                } else {
                    text = text + words[i];
                }
            }
            
            return text;
        }
        
        public AttributeSet getColorAttribute() {
            StyleContext cont = StyleContext.getDefaultStyleContext();
            return cont.addAttribute(cont.getEmptySet(), StyleConstants.Foreground, color);
        }

    }
    
    /**
     * Sets color of all integers
     * 
     * @param color
     */
    public void setIntegerColours(Color color) {
        defaultNumbers = cont.addAttribute(cont.getEmptySet(), StyleConstants.Foreground, color);
        
    }
    
    /**
     * Sets color of non-keywords 
     * 
     * @param color
     */
    public void setDefaultTextColour(Color color) {
        defaultForeground = cont.addAttribute(cont.getEmptySet(), StyleConstants.Foreground, color);
    }
    
    /**
     * Initializes rules by which textpane should be coloring text
     */
    public void initializeSyntaxHighlighter() {
        // Set background color
        //setBackground(new Color(61, 60, 63));
        
        
        addKeyWord(new Color(191, 103, 47),
               "while",
               "if",
               "else",
               "print"
                );
        
        addKeyWord(new Color(30, 134, 186),
                "\\?",
                "\\=",
                "\\<",
                "\\>",
                "\\?",
                "\\*",
                "\\+",
                "\\-",
                "\\/",
                "\\!"
                );
        addKeyWord(new Color(222, 90, 83),
                "\\&",
                "\\|"
                );
        
        
    }
    
    

}