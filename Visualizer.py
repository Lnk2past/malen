import itertools
from bokeh.io import curdoc
from bokeh.layouts import row, column
from bokeh.models import HoverTool, CustomJS, Slider
from bokeh.plotting import figure, output_file, save, ColumnDataSource
from bokeh.palettes import viridis as palette

colors = itertools.cycle(palette(16))

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
    fig = figure(**locals())
    return fig


def plot(fig, plot_type, x, y, **kwargs):
    """Create a Glyph Renderer

    Arguments:
        plot_type (str): type of plot to make
        x (list): x data
        y (list): y data
        kwargs: Line properties and Fill properties
    """
    if plot_type == 'scatter':
        plot_type = 'circle'

    source = ColumnDataSource(data=dict(x=x, y=y))
    if plot_type == 'line':
        return fig.line(x='x', y='y', source=source, color=colors.__next__(), **kwargs)
    elif plot_type == 'vbar':
        kwargs['width'] = kwargs.get('width', 1)
        return fig.line(x='x', top='y', source=source, color=colors.__next__(), **kwargs)
    else:
        return fig.scatter(x='x', y='y', source=source, marker=plot_type, color=colors.__next__(), **kwargs)


def plot_color(fig, plot_type, x, y, c, **kwargs):
    """Create a Glyph Renderer

    Arguments:
        plot_type (str): type of plot to make
        x (list): x data
        y (list): y data
        c (list): color data
        kwargs: Line properties and Fill properties
    """
    if plot_type == 'scatter':
        plot_type = 'circle'

    p = palette(max(c))
    source = ColumnDataSource(data=dict(x=x, y=y, color=[p[i-1] for i in c]))
    if plot_type == 'line':
        print('plot_color does not support line plots')
        return None
    elif plot_type == 'vbar':
        kwargs['width'] = kwargs.get('width', 1)
        return fig.vbar(x='x', top='y', color='color', source=source, **kwargs)
    else:
        return fig.scatter(x='x', y='y', color='color', marker=plot_type, source=source, **kwargs)


def image(fig, image, **kwargs):
    """
    """
    source = ColumnDataSource(data=dict(image=[image], x=[0], y=[0], dw=[len(image[0])], dh=[len(image)]))
    max_r = max(len(image[0]), len(image))
    fig.x_range.start = 0
    fig.x_range.end = max_r
    fig.y_range.start = 0
    fig.y_range.end = max_r

    kwargs['palette'] = kwargs.get('palette', 'Spectral11')
    return fig.image(image='image', x='x', y='y', dw='dw', dh='dh', source=source, **kwargs)


def image_slider(fig, images, **kwargs):
    x = [0 for _ in images[0]]
    y = [0 for _ in images]
    dw = [len(image[0]) for image in images]
    dh = [len(image) for image in images]

    image_renderer = image(fig, images[0], **kwargs)

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
    return column(fig, row(slider))


def generate_html(obj, filename, **kwargs):
    """Generate and save the HTML of the figure

    Arguments:
        obj: Bokeh object or figure to be graphed
        filename (str): name of the file to save the figure to
        kwargs: additional output properties
    """
    output_file(filename, **kwargs)
    save(obj)

if __name__ == '__main__':
    import random
    x = [random.randint(0,10) for _ in range(100)]
    y = [random.randint(0,10) for _ in range(100)]
    f = make_new_figure('test')
    s = plot(f, 'scatter', x, y)
    generate_html(f, 'test.html')