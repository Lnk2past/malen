import itertools
from bokeh.io import curdoc
from bokeh.layouts import row, column
from bokeh.models import HoverTool, CustomJS, Slider
from bokeh.plotting import figure, output_file, save, ColumnDataSource
from bokeh.palettes import Dark2_5 as palette
colors = itertools.cycle(palette)

layout = None

def make_new_figure(title, plot_width=640, plot_height=640):
    """Create a new figure
    
    Arguments:
        title (str): name of the figure
        plot_width (int): width in pixels
        plot_height (int): height in pixels

    Returns:
        bokeh.plotting.figure.Figure object to be graphed on
    """
    fig = figure(x_range=(0, 10), y_range=(0, 10), **locals())
    return fig


def line(fig, x, y, **kwargs):
    """Create a line object
    
    Arguments:
        x (list): x data
        y (list): y data
        kwargs: Line properties and Fill properties
    """
    source = ColumnDataSource(data=dict(x=x, y=y))
    return fig.line(x='x', y='y', source=source, color=colors.__next__(), line_width=1, **kwargs)


def circle(fig, x, y, **kwargs):
    """Create a circle object
    
    Arguments:
        x (list): x data
        y (list): y data
        kwargs: Line properties and Fill properties
    """
    source = ColumnDataSource(data=dict(x=x, y=y))
    return fig.circle(x='x', y='y', source=source, color=colors.__next__(), **kwargs)


def vbar(fig, x, y, width, **kwargs):
    """Create a vbar object
    
    Arguments:
        x (list): x data
        y (list): bin values for each bar
        kwargs: Line properties and Fill properties
    """
    source = ColumnDataSource(data=dict(x=x, top=y))
    return fig.vbar(x=x, top=y, width=width, fill_color='pink', fill_alpha=0.75, **kwargs)


def image(fig, image, **kwargs):
    """
    """
    source = ColumnDataSource(data=dict(image=[image], x=[0], y=[0], dw=[len(image[0])], dh=[len(image)]))
    return fig.image(image='image', x='x', y='y', dw='dw', dh='dh', source=source, palette="Spectral11")


def image_slider(fig, images):
    x = [0 for _ in images[0]]
    y = [0 for _ in images]
    dw = [len(image[0]) for image in images]
    dh = [len(image) for image in images]

    image_renderer = image(fig, images[0])

    source2 = ColumnDataSource(data=dict(image=images, x=x, y=y, dw=dw, dh=dh))
    slider = Slider(start=0, end=(len(images)-1), value=0, step=1, title="Frame")
    callback = CustomJS(args=dict(source=image_renderer.data_source, source2=source2),
                        code="""
source.data['image'] = [source2.data['image'][cb_obj.value]];
source.data['x']     = [source2.data['x'][cb_obj.value]];
source.data['y']     = [source2.data['y'][cb_obj.value]];
source.data['dw']    = [source2.data['dw'][cb_obj.value]];
source.data['dh']    = [source2.data['dh'][cb_obj.value]];
source.change.emit();""")

    slider.js_on_change('value', callback)
    return make_layout(fig, slider)

def make_layout(fig, obj):
    return column(fig, row(obj))

def generate_html(obj, filename, **kwargs):
    """Generate and save the HTML of the figure
    cd o    
    Arguments:
        filename (str): name of the file to save the figure to
        kwargs: additional output properties
    """
    output_file(filename, **kwargs)
    save(obj)


if __name__ == '__main__':
    import random
    data = []
    for i in range(10):
        data.append([])
        for _ in range(10):
            data[-1].append([random.randint(0,10), random.randint(0,10)])

    f = make_new_figure('test')
    s = image_slider(f, data)
    l = make_layout(f, s)
    generate_html(l, 'test.html')
