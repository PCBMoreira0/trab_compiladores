#include "afn_builder.h"
#include "value_stack.h"

#define CONCAT_OPERATOR '&'
#define UNION_OPERATOR '|'
#define KLEENE_OPERATOR '*'
#define PLUS_KLEENE_OPERATOR '^'


int afnBuildFromER(AFN_Context *ctx, const char *regex, AFN_Fragment *out_fragment) {
    ValueStack *stack = value_stack_create(sizeof(AFN_Fragment));
    for(int i = 0; regex[i] != '\0'; i++) {
        char c = regex[i];
        
        switch(c){
            case UNION_OPERATOR:
                AFN_Fragment frag_union1, frag_union2;
                value_stack_pop(stack, &frag_union2);
                value_stack_pop(stack, &frag_union1);
                AFN_Fragment union_afn = afnCreateUnion(ctx, frag_union2, frag_union1);
                value_stack_push(stack, &union_afn);
                break;
            case KLEENE_OPERATOR:
                AFN_Fragment frag_kleene;
                value_stack_pop(stack, &frag_kleene);
                AFN_Fragment kleene_afn = afnCreateKleene(ctx, frag_kleene);
                value_stack_push(stack, &kleene_afn);
                break;
            case CONCAT_OPERATOR:
                AFN_Fragment frag_concat1, frag_concat2;
                value_stack_pop(stack, &frag_concat2);
                value_stack_pop(stack, &frag_concat1);
                AFN_Fragment concat_afn = afnCreateConcat(ctx, frag_concat1, frag_concat2);
                value_stack_push(stack, &concat_afn);
                break;
            case PLUS_KLEENE_OPERATOR:
                AFN_Fragment frag_plus;
                value_stack_pop(stack, &frag_plus);
                AFN_Fragment kleene_plus_afn = afnCreateKleene(ctx, frag_plus);
                AFN_Fragment concat_plus_afn = afnCreateConcat(ctx, frag_plus, kleene_plus_afn);
                value_stack_push(stack, &concat_plus_afn);
                break;
            default:
                AFN_Fragment frag_symbol = afnCreateSymbol(ctx, c);
                value_stack_push(stack, &frag_symbol);
                break;
        }
    }
    
    AFN_Fragment result;
    value_stack_pop(stack, &result);
    int isEmpty = value_stack_is_empty(stack);
    value_stack_destroy(stack);
    if(!isEmpty) {
        return 0; // ER inválida, mais de um fragmento restante
    }
    
    *out_fragment = result;
    return 1; // Retornar o fragmento do AFN construído
}