from django.shortcuts import render

def runoob(request):
    context = {}
    listd = ['a', 'b', 'c']
    context['hello'] = listd
    return render(request, 'runoob.html', context)
